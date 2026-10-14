# Name: DataFrameColumnMatcher

import inviwopy as ivw
import ivwdataframe


class DataFrameColumnMatcher(ivw.Processor):
    """
    This processor matches and synchronizes a StringProperty with a ColumnOptionProperty of
    the input DataFrame.
    """

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.inport = ivwdataframe.DataFrameInport("inport")
        self.addInport(self.inport)

        self.columnHeader = ivw.properties.StringProperty(
            "columnHeader", "Column Name",
            help=ivw.md2doc("Header of the DataFrame column that should be selected"))
        self.column = ivwdataframe.ColumnOptionProperty(
            "column", "DataFrame Column",
            help=ivw.md2doc("Selected DataFrame column"),
            port=self.inport,
            noneOption=ivwdataframe.AddNoneOption.Yes)

        self.rangeMinMax = ivw.properties.DoubleMinMaxProperty(
            "rangeMinMax", "Range MinMax", 0.0, 100.0, 0.0, 100.0, 0.001, 0.0)

        self.addProperties([self.columnHeader, self.column])

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.DataFrameColumnMatcher",
            displayName="DataFrame Column Matcher",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags([ivw.Tags.PY, ivw.Tag("DataFrame"), ivw.Tag("InfraVis")]),
            help=ivw.unindentMd2doc(DataFrameColumnMatcher.__doc__)
        )

    def getProcessorInfo(self):
        return DataFrameColumnMatcher.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        if not self.columnHeader.value:
            return

        dataframe = self.inport.getData()
        header = self.columnHeader.value
        colIndex = next((i for i, col in enumerate(dataframe) if col.header == header), None)

        if colIndex is None:
            raise ValueError(f"Could not locate column '{header}' in DataFrame")
        if self.column.hasNoneOption():
            colIndex += 1

        self.column.selectedIndex = colIndex
