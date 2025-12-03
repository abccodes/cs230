---
title: Report
---


{% capture template %}



<div class="section">
    <h1>Experiment Configuration</h1>
    <ul class="browser-default">
        <li><b>Duration</b>: 1w</li>
        <li><b>Trials</b>: 10</li>
        <li><b>Version</b>: v1.0</li>
    </ul>
</div>
<div class="section">
    <h1>Fuzz Targets</h1>
    <p>This report summarizes the results of fuzzing the following targets:</p>
    <ul id="target-list" class="browser-default">
        
        <li><a href="targets/libtiff.html">libtiff</a></li>
        
    </ul>
</div>

<div class="section">
    <h1>Evaluated Fuzzers</h1>
    <p>The fuzzers used in this evaluation are listed below:</p>
    <ul id="fuzzer-list" class="browser-default">
        
        <li><a href="fuzzers/aflplusplus.html">aflplusplus</a></li>
        
    </ul>
</div>

<!--
EXPERIMENT PARAMETERS WILL BE LISTED HERE
-->

<div class="section">
    <h1>Experiment Summary</h1>
    <div id="bugs-triggered">
        <h2>Total Unique Bugs Triggered</h2>
        <ul class="collapsible popout">
            <li>
                <div class="collapsible-header">
                    <h3>Mean and Standard Deviation</h3>
                </div>
                <div class="collapsible-body">
                    This plot shows the mean number of unique bugs triggered by every fuzzer against every target,
                    and the standard deviation bar, across all campaigns.
                </div>
            </li>
        </ul>
        <img class="materialboxed responsive-img" src="plot/summary_unique_bugs.svg">
        <ul class="collapsible popout">
            <li>
                <div class="collapsible-header">
                    <h3>Stastistical Significance</h3>
                </div>
                <div class="collapsible-body">
                    This matrix summarizes the p-values of the pairwise Mann-Whitney U tests calculated against the
                    total bug count sample sets collected for every fuzzer across all campaigns. Cells with a green
                    shade indicate that the number of bugs triggered by a fuzzer is statistically different.
                </div>
            </li>
        </ul>
        <img class="materialboxed responsive-img" src="plot/summary_signplot.svg">
    </div>

    <div id="mean-survival">
        <h2>Mean Survival Time</h2>
        <ul class="collapsible popout">
            <li>
                <div class="collapsible-header">
                    <h3>Ranking of Bugs and Fuzzers</h3>
                </div>
                <div class="collapsible-body">
                    This table shows the restricted mean survival time for every bug being reached or triggered over the
                    duration of the campaign, using the Kaplan-Meier non-parametric survival function estimator.
                    Green-shaded cells indicate the best performance (lowest time) for a bug metric across all fuzzers.
                    Yellow-shaded cells indicate the worst performance (highest time) for a bug metric across all fuzzers.
                    Red-shaded cells indicate that the bug survived being reached or triggered by the fuzzer throughout
                    the campaign duration.
                </div>
            </li>
        </ul>
        <div class="center">
            <a id="btn-colormap" class="btn-small waves-effect waves-light" style="margin-bottom: 10px; color: white; font-weight: normal;">
                <i class="material-icons left">color_lens</i>
                Change Colormap
            </a>
        </div>
        <script type="text/javascript">
            $('#btn-colormap').click(function() {
                stl = $('#survival_stylesheet');
                var href = stl.prop('href');
                if (href.includes('hiliter')) {
                    stl.prop('href', 'css/survival_heatmap.css');
                } else {
                    stl.prop('href', 'css/survival_hiliter.css');
                }
            });
        </script>
        <link id="survival_stylesheet" rel="stylesheet" href="css/survival_hiliter.css">
        <style type="text/css">
#T_survival_table tr:nth-child(even) {
  background-color: #ececec;
}
#T_survival_table tbody tr:hover {
  background-color: #d9edfd;
}
#T_survival_table thead tr:not(:last-child) .index_name {
  visibility: hidden;
}
#T_survival_table thead tr:nth-child(3) {
  display: none;
}
#T_survival_table tbody td {
  padding: 2px 5px 2px 15px;
  text-align: right;
}
#T_survival_table th {
  padding: 2px 5px 2px 5px;
  text-align: center;
}
#T_survival_table th {
  border-top: none;
  border-bottom: none;
  border-radius: 0px;
}
#T_survival_table  td {
  border-top: none;
  border-bottom: none;
  border-radius: 0px;
}
#T_survival_table  tr {
  border-top: none;
  border-bottom: none;
  border-radius: 0px;
}
#T_survival_table thead tr:first-child th:not(:last-child) {
  border-right: 1px solid #888888;
}
#T_survival_table  th:nth-child(odd):not(:last-child) {
  border-right: 1px solid #888888;
}
#T_survival_table  td:nth-child(odd):not(:last-child) {
  border-right: 1px solid #888888;
}
#T_survival_table  {
  border-collapse: collapse;
}
</style>
<table id="T_survival_table">
  <thead>
    <tr>
      <th class="blank level0" >&nbsp;</th>
      <th id="T_survival_table_level0_col0" class="col_heading level0 col0" colspan="2">aflplusplus</th>
    </tr>
    <tr>
      <th class="blank level1" >&nbsp;</th>
      <th id="T_survival_table_level1_col0" class="col_heading level1 col0" >R</th>
      <th id="T_survival_table_level1_col1" class="col_heading level1 col1" >T</th>
    </tr>
    <tr>
      <th class="index_name level0" >Bug ID</th>
      <th class="blank col0" >&nbsp;</th>
      <th class="blank col1" >&nbsp;</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th id="T_survival_table_level0_row0" class="row_heading level0 row0" >TIF007</th>
      <td id="T_survival_table_row0_col0" class="data row0 col0" >15s</td>
      <td id="T_survival_table_row0_col1" class="data row0 col1" >25s</td>
    </tr>
    <tr>
      <th id="T_survival_table_level0_row1" class="row_heading level0 row1" >TIF012</th>
      <td id="T_survival_table_row1_col0" class="data row1 col0" >10s</td>
      <td id="T_survival_table_row1_col1" class="data row1 col1" >2m</td>
    </tr>
    <tr>
      <th id="T_survival_table_level0_row2" class="row_heading level0 row2" >TIF014</th>
      <td id="T_survival_table_row2_col0" class="data row2 col0" >15s</td>
      <td id="T_survival_table_row2_col1" class="data row2 col1" >5m</td>
    </tr>
    <tr>
      <th id="T_survival_table_level0_row3" class="row_heading level0 row3" >TIF005</th>
      <td id="T_survival_table_row3_col0" class="data row3 col0" >11m</td>
      <td id="T_survival_table_row3_col1" class="data row3 col1" >11m</td>
    </tr>
    <tr>
      <th id="T_survival_table_level0_row4" class="row_heading level0 row4" >TIF006</th>
      <td id="T_survival_table_row4_col0" class="data row4 col0" >11m</td>
      <td id="T_survival_table_row4_col1" class="data row4 col1" >11m</td>
    </tr>
    <tr>
      <th id="T_survival_table_level0_row5" class="row_heading level0 row5" >TIF002</th>
      <td id="T_survival_table_row5_col0" class="data row5 col0" >1h</td>
      <td id="T_survival_table_row5_col1" class="data row5 col1" >5h</td>
    </tr>
    <tr>
      <th id="T_survival_table_level0_row6" class="row_heading level0 row6" >TIF004</th>
      <td id="T_survival_table_row6_col0" class="data row6 col0" >1h</td>
      <td id="T_survival_table_row6_col1" class="data row6 col1" >1w</td>
    </tr>
    <tr>
      <th id="T_survival_table_level0_row7" class="row_heading level0 row7" >TIF003</th>
      <td id="T_survival_table_row7_col0" class="data row7 col0" >15s</td>
      <td id="T_survival_table_row7_col1" class="data row7 col1" >1w</td>
    </tr>
    <tr>
      <th id="T_survival_table_level0_row8" class="row_heading level0 row8" >TIF010</th>
      <td id="T_survival_table_row8_col0" class="data row8 col0" >15m</td>
      <td id="T_survival_table_row8_col1" class="data row8 col1" >1w</td>
    </tr>
  </tbody>
</table>
    </div>
</div>



{% endcapture %}
{{ template | replace: '    ', ''}}
