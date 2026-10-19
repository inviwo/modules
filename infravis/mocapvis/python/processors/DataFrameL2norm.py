# Name: DataFrameL2Norm

import inviwopy as ivw
import ivwdataframe as df

import numpy as np


class DataFrameL2Norm(ivw.Processor):
    """
    Compute the L2 norm of two or more columns and add the result as a new column
    """

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.inport = df.DataFrameInport(
            "dataframe",
            help=ivw.md2doc('Input DataFrame')
        )
        self.addInport(self.inport)
        self.outport = df.DataFrameOutport(
            "output",
            help=ivw.md2doc('Resulting DataFrame')
        )
        self.addOutport(self.outport)

        self.columnNames = ivw.properties.StringProperty(
            "columnNames", "Columns",
            ivw.md2doc("List of column names to consider. Use Semicolon `;` as separator"))
        self.resultColumn = ivw.properties.StringProperty(
            "resultColumn", "Result",
            ivw.md2doc("Header of the resulting column"),
            "Distance")
        self.addProperties([self.columnNames, self.resultColumn])

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.DataFrameL2Norm",
            displayName="DataFrameL2Norm",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags.PY | ivw.Tag("DataFrame"),
            help=ivw.unindentMd2doc(DataFrameL2Norm.__doc__)
        )

    def getProcessorInfo(self):
        return DataFrameL2Norm.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        if not self.columnNames.value.strip():
            raise ValueError(f'No source columns given.')
            return

        source = self.inport.getData()

        dataframe = df.DataFrame()
        #  reuse indices from source DataFrame
        dataframe.indexcol().append(source.indexcol())
        for column in source:
            if column is not source.indexcol():
                dataframe.addColumn(column)

        squared_distances = np.zeros((dataframe.rows))

        for col_name in [s.strip() for s in self.columnNames.value.split(';')]:
            column = dataframe.getColumn(name=col_name)
            if not column:
                raise ValueError(f'Column "{col_name}" not found in input DataFrame.')
            if not column.type == df.ColumnType.Ordinal:
                raise ValueError(
                    f'Expected column "{col_name}" to be of ordinal type. Found "{column.type}".')

            squared_distances += np.square(column.buffer.data)

        dataframe.addColumnFromBuffer(self.resultColumn.value, ivw.data.Buffer(np.sqrt(squared_distances)))
        dataframe.updateIndex()

        self.outport.setData(dataframe)
