# Name: StreamFunctionSolver

from cmath import inf
from sys import get_coroutine_origin_tracking_depth
import inviwopy as ivw
from inviwopy.properties import IntProperty  # FileProperty, ButtonProperty, BoolProperty, \
# BoolCompositeProperty, CompositeProperty, IntMinMaxProperty, StringProperty
# import scipy
import numpy as np
import scipy.sparse
import scipy.sparse.linalg


class StreamFunctionSolver(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.streamFunctionOut = ivw.data.ImageOutport("streamFunction")
        self.vectorFieldIn = ivw.data.ImageInport("vectorField")
        self.vectorFieldIn.setOutportDeterminesSize(True)
        self.initalGuess = ivw.data.ImageInport("initalGuess")
        self.initalGuess.setOutportDeterminesSize(True)
        self.initalGuess.setOptional(True)
        self.streamFunctionOut.setHandleResizeEvents(False)
        self.addOutport(self.streamFunctionOut)
        self.addInport(self.vectorFieldIn)
        self.addInport(self.initalGuess)
        self.maxIter = ivw.properties.IntProperty(
            "maxIter", "Max Iterations", 10, 1, 10000)
        self.addProperty(self.maxIter)

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.vectorvis.StreamFunctionSolver",
            displayName="Stream Function Solver",
            category="VectorVis",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags([ivw.Tag.PY, ivw.Tag("Image")])
        )

    def getProcessorInfo(self):
        return StreamFunctionSolver.processorInfo()

    def process(self):
        if (not self.vectorFieldIn.hasData()):
            print("No field :(")
            return

        vectorField = self.vectorFieldIn.getData()
        vectorData = vectorField.colorLayers[0].data

        fullFieldSize = vectorData.shape
        fieldSize = fullFieldSize[:2]
        numElements = fieldSize[0] * fieldSize[1]

        vectorDataX = vectorData[:, :, 0].squeeze()
        vectorDataY = vectorData[:, :, 1].squeeze()
        result = vectorDataX.copy()

        # Get indices of all valid cells.
        validCells = np.where(vectorDataX != 0)
        numValidCells = len(validCells[0])

        # Build linear system defining the gradients. Sparsely!
        # Let's use the scipy sparse COO matrix.
        # Thus, first build the index and value arrays.
        #
        # First, assume all just use their neighbors!
        gradDir = (np.array(validCells[0]), np.array(
            validCells[0]), np.array(validCells[1]), np.array(validCells[1]))

        # Gradient in X.
        print('at border 0: ', gradDir[0]
              [(gradDir[0] == fieldSize[0]-1)].shape[0])
        gradDir[0][(gradDir[0] < fieldSize[0]-1)] += 1

        print('bad neighbor 0: ', gradDir[0][vectorDataX[(
            gradDir[0], validCells[1])] == 0].shape[0])
        gradDir[0][vectorDataX[(gradDir[0], validCells[1])] == 0] -= 1

        gradDir[1][((gradDir[1]) > 0)] -= 1
        gradDir[1][vectorDataX[(gradDir[1], validCells[1])] == 0] += 1

        scaleX = (gradDir[0] - gradDir[1])
        print('invalid in scaleX: ', scaleX[scaleX == 0].shape[0])

        # Gradient in Y.
        gradDir[2][((gradDir[2]+1) < fieldSize[1])] += 1
        gradDir[2][vectorDataX[(validCells[0], gradDir[2])] == 0] -= 1

        gradDir[3][((gradDir[3]) > 0)] -= 1
        gradDir[3][vectorDataX[(validCells[0], gradDir[3])] == 0] += 1

        scaleY = (gradDir[2] - gradDir[3])
        print('invalid in scaleY: ', scaleY[scaleY == 0].shape[0])
        print('forward in scaleY: ',
              scaleY[scaleY == 1].shape[0], 'of', scaleY.shape[0])

        # Coefficient are scaled by (inverse) index distance.
        coeffs = np.concatenate((scaleX, scaleY), axis=None)

        # Linear indices for coefficient matrix
        linearIndexLookup = np.zeros(vectorDataX.shape, dtype=int)
        linearIndexLookup[validCells] = np.arange(0, numValidCells, dtype=int)
        linIdxs = (linearIndexLookup[(gradDir[0], validCells[1])], linearIndexLookup[(validCells[0], gradDir[2])], linearIndexLookup[(
            gradDir[1], validCells[1])], linearIndexLookup[(validCells[0], gradDir[3])])
        rows = np.concatenate(linIdxs, axis=None)

        # Gradient vector from input field.
        gradientVector = np.append(
            vectorDataY[validCells], -vectorDataX[validCells])

        numValidEquations = coeffs[coeffs != 0].shape[0]
        print('Out of', numValidCells*2, 'equations,', numValidEquations,
              'are valid (', numValidCells*2-numValidEquations, 'invalid)')

        gradientVector = gradientVector[coeffs != 0]
        coeffs = np.append(coeffs, -coeffs, axis=None)
        validEquations = coeffs != 0
        rows = rows[validEquations]
        coeffs = coeffs[validEquations]
        coeffs = 1.0 / coeffs
        columns = np.tile(np.arange(0, numValidEquations, 1), 2)

        print('Rows:    ', rows.shape)
        print('Columns: ', columns.shape)
        print('Coeffs:  ', coeffs.shape)
        print('Gradient:', gradientVector.shape)

        # Build sparse matrix.
        gradientMatrix = scipy.sparse.coo_matrix((coeffs, (columns, rows)), shape=(
            numValidEquations, numValidCells))
        print(gradientMatrix.shape, '* x = ', gradientVector.shape)

        # Inital guess.
        guess = np.zeros((numValidCells))
        if self.initalGuess.hasData():
            initalImag = self.initalGuess.getData()
            initalData = initalImag.colorLayers[0].data
            guess = initalData[validCells]

        solved = scipy.sparse.linalg.lsqr(
            gradientMatrix, gradientVector, iter_lim=self.maxIter.value, x0=guess)
        # print(type(streamFunction[0]), streamFunction[0].shape)
        print('Stopped because of', solved[1], 'after', solved[2], 'steps')

        result[vectorDataY == 0] = -100
        result[validCells] = solved[0]
        fieldSize = np.flip(fieldSize)  # URRRGH! BIG URRRRGH!
        imageLayer = ivw.data.Layer(result.reshape(fieldSize))
        image = ivw.data.Image(imageLayer)
        self.streamFunctionOut.setData(image)

        # At some point, solve: np.linalg.lstsq(gradientOperator, perpendicularVectorField)
