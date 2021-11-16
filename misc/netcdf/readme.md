# netcdf Module

Utilities for loading and handling NetCDF data. The module relies on the python NetCDF 4 library, but can handle older formats as well.
Nested variables in groups are not supported so far.

To install the python NetCDF4 dependency run `pip install netcdf4`.

### Requires Python 3.9
For reference (in case you cannot upgrade to Python 3.9):
Another solution is to import the `typing` library in genericnetcdfsource.py:
```python
from typing import List
```
and replace `list[Dimension]` with `List[Dimension]`.
