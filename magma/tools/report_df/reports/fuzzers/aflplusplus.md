---
title: aflplusplus
---


{% capture template %}



<div class="section">
    <h1>aflplusplus</h1>
    <p>
        This page shows the distribution of time-to-bug measurements for every bug reached and/or triggered by the
        fuzzer. The results are grouped by target to highlight any performance trends the fuzzer may have against
        specific targets.
    </p>

    
    <h2>libtiff</h2>
    
        
        <h3>tiffcp</h3>
        <div class="row">
        
            
            <div class="col s6">
                <img class="materialboxed responsive-img" src="../plot/box_aflplusplus_libtiff_tiffcp_reached.svg">
            </div>
        
            
            <div class="col s6">
                <img class="materialboxed responsive-img" src="../plot/box_aflplusplus_libtiff_tiffcp_triggered.svg">
            </div>
        
        </div>
    

</div>



{% endcapture %}
{{ template | replace: '    ', ''}}
