#include <inviwo/tensorvisio/processors/amiratensorreader.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo AmiraTensorReader::processorInfo_{
    "org.inviwo.AmiraTensorReader",  // Class identifier
    "Amira Tensor Reader",           // Display name
    "Tensor Field IO",               // Category
    CodeState::Experimental,         // Code state
    tag::OpenTensorVis | Tag::CPU,   // Tags
};
const ProcessorInfo AmiraTensorReader::getProcessorInfo() const { return processorInfo_; }

AmiraTensorReader::AmiraTensorReader()
    : Processor()
    , inFile_("inFile", "File", "")
    //, outport3D_("outport3D")
    , outport_("outportRaw") {
    addProperty(inFile_);
    // addPort(outport3D_);
    addPort(outport_);
}

void AmiraTensorReader::process() {
    // const char* FileName = "testscalar.am";
    const char* FileName = inFile_.get().c_str();
    // const char* FileName = "testvector3c.am";

    FILE* fp = fopen(FileName, "rb");
    if (!fp) {
        LogError("Could not find " << FileName);
        return;
    }

    LogInfo("Reading " << FileName);

    // We read the first 2k bytes into memory to parse the header.
    // The fixed buffer size looks a bit like a hack, and it is one, but it gets the job done.
    char buffer[2048];
    if (fread(buffer, sizeof(char), 2047, fp) != 2047) return;
    buffer[2047] = '\0';  // The following string routines prefer null-terminated strings

    if (!strstr(buffer, "# AmiraMesh BINARY-LITTLE-ENDIAN 2.1")) {
        LogError("Not a proper AmiraMesh file.\n");
        fclose(fp);
        return;
    }

    // Find the Lattice definition, i.e., the dimensions of the uniform grid
    int xDim(0), yDim(0), zDim(0);
    sscanf(FindAndJump(buffer, "define Lattice"), "%d %d %d", &xDim, &yDim, &zDim);
    printf("\tGrid Dimensions: %d %d %d\n", xDim, yDim, zDim);

    // Find the BoundingBox
    float xmin(1.0f), ymin(1.0f), zmin(1.0f);
    float xmax(-1.0f), ymax(-1.0f), zmax(-1.0f);
    sscanf(FindAndJump(buffer, "BoundingBox"), "%g %g %g %g %g %g", &xmin, &xmax, &ymin, &ymax,
           &zmin, &zmax);
    LogInfo("BoundingBox in x-Direction: [" << xmin << " ... " << xmax << "]\n");
    LogInfo("BoundingBox in y-Direction: [" << ymin << " ... " << ymax << "]\n");
    LogInfo("BoundingBox in z-Direction: [" << zmin << " ... " << zmax << "]\n");

    auto extents = vec3(xmax - xmin, ymax - ymin, zmax - zmin);

    // Type of the field: scalar, vector
    int NumComponents(0);
    if (strstr(buffer, "Lattice { float Data }")) {
        // Scalar field
        NumComponents = 1;
    } else {
        // A field with more than one component, i.e., a vector field
        sscanf(FindAndJump(buffer, "Lattice { float["), "%d", &NumComponents);
    }
    LogInfo("Number of Components: " << NumComponents);

    // Find the beginning of the data section
    const long idxStartData = long(strstr(buffer, "# Data section follows") - buffer);
    if (idxStartData > 0) {
        // Set the file pointer to the beginning of "# Data section follows"
        fseek(fp, idxStartData, SEEK_SET);
        // Consume this line, which is "# Data section follows"
        if (!fgets(buffer, 2047, fp)) return;
        // Consume the next line, which is "@1"
        if (!fgets(buffer, 2047, fp)) return;

        // Read the data
        // - how much to read
        const size_t NumToRead = xDim * yDim * zDim * NumComponents;
        // - prepare memory; use malloc() if you're using pure C
        float* pData = new float[NumToRead];
        if (pData) {
            // - do it
            const size_t ActRead = fread((void*)pData, sizeof(float), NumToRead, fp);
            // - ok?
            if (NumToRead != ActRead) {
                LogError(
                    "Something went wrong while reading the binary data section.\nPremature end of "
                    "file?\n");
                delete[] pData;
                fclose(fp);
                return;
            }

            // Test: Print all data values
            // Note: Data runs x-fastest, i.e., the loop over the x-axis is the innermost
            printf("\nPrinting all values in the same order in which they are in memory:\n");
            int Idx(0);

            std::vector<mat3> data;

            for (int z = 0; z < zDim; z++) {
                for (int y = 0; y < yDim; y++) {
                    for (int x = 0; x < xDim; x++) {
                        auto dataPoint1 = pData[Idx * NumComponents];
                        auto dataPoint2 = pData[Idx * NumComponents + 1];
                        auto dataPoint3 = pData[Idx * NumComponents + 2];
                        auto dataPoint4 = pData[Idx * NumComponents + 3];
                        auto dataPoint5 = pData[Idx * NumComponents + 4];
                        auto dataPoint6 = pData[Idx * NumComponents + 5];

                        auto tensor = mat3(vec3(dataPoint1, dataPoint2, dataPoint3),
                                           vec3(dataPoint2, dataPoint4, dataPoint5),
                                           vec3(dataPoint3, dataPoint5, dataPoint6));

                        data.push_back(tensor);

                        Idx++;
                    }
                }
            }

            delete[] pData;

            auto newTF = std::make_shared<TensorField3D>(size3_t(xDim, yDim, zDim), data);
            newTF->setExtents(extents);

            outport_.setData(newTF);
        }
    }

    fclose(fp);
}

}  // namespace inviwo
