Highcharts Pane Module
===============

*Note: This module is still in the development phase*

## Dependencies
* Highcharts Library, at least version 2.3.5 (http://www.highcharts.com/download)
* Ctools
* Libraries

## Usage
1. Enable the module
2. Add the highcharts pane to a panel page. It can be found under the "Charts" group.
3. Fill out all the text fields and select your chart type
   * Currently only Bar (http://www.highcharts.com/demo/bar-basic), Line (http://www.highcharts.com/demo/line-basic), and Column (http://www.highcharts.com/demo/column-basic) are supported
4. For the CSV file, your data should be of the format:

| Column 1   | Column 2   | Column 3      | etc. |
| --------   | ---------- | ------------- | ---- |
| Category 1 | Category 2 | Category 3    | etc. |
| Name 1     | Data 1     | Data 1        | etc. |
| Name 2     | Data 2     | Data 2        | etc. |

  * The Categories are what will be used across the X-Axis
  * The Name items are what will be used in the legend

## Known Issues
* Can't have more than 1 chart in a page
* Still need to implement admin info
* Currently not accessible
  * Will be adding a table of the data to help with this
* File isn't deleted if pane is removed, and has no usage anywhere else (Not sure if this is necissarily an issue)

In the future, I plan to add more chart types

  
