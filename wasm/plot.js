function plot(data1, data2) {

    let margin = histogram_config.margin;
    let width = histogram_config.width;
    let height = histogram_config.height;
    let svg = histogram_config.svg;

    // X axis: scale and draw:
    let xm = Math.max(Math.max(...data1), Math.max(...data2));
    var x = d3.scaleLinear()
        .domain([0, xm*2]) 
        .range([0, width]);
    svg.append("g")
        .attr("transform", "translate(0," + height + ")")
        .call(d3.axisBottom(x));

    // set the parameters for the histogram
    var histogram = d3.histogram()
        .value(function(d) { return d; })   // I need to give the vector of value
        .domain(x.domain())  // then the domain of the graphic
        .thresholds(x.ticks(100)); // then the numbers of bins

    // And apply this function to data to get the bins
    var bins1 = histogram(data1);
    var bins2 = histogram(data2);
    var bins_total = bins1.concat(bins2);

    // Y axis: scale and draw:
    var y = d3.scaleLinear()
        .range([height, 0]);
    y.domain([0, d3.max(bins_total, function(d) { return d.length; })]);   // d3.hist has to be called before the Y axis obviously
    svg.append("g")
        .call(d3.axisLeft(y));

    // append the bar rectangles to the svg element
    svg.selectAll("rect1")
        .data(bins1)
        .enter()
        .append("rect")
            .attr("x", 1)
            .attr("transform", function(d) { return "translate(" + x(d.x0) + "," + y(d.length) + ")"; })
            .attr("width", function(d) { return x(d.x1) - x(d.x0); })
            .attr("height", function(d) { return height - y(d.length); })
            .style("fill", "#0000ff")
            .style("opcaity", 0.9);
            
    svg.selectAll("rect2")
        .data(bins2)
        .enter()
        .append("rect")
            .attr("x", 1)
            .attr("transform", function(d) { return "translate(" + x(d.x0) + "," + y(d.length) + ")"; })
            .attr("width", function(d) { return x(d.x1) - x(d.x0); })
            .attr("height", function(d) { return height - y(d.length); })
            .style("fill", "#ff0000")
            .style("opcaity", 0.9);
}

function run_test() {
    document.getElementById("caption").innerText = "Running..."
    let [without_x, with_x] = tlb_get_data();
    plot(without_x, with_x);
    document.getElementById("caption").innerText = "Done!"
}
  
function init_histogram() {
    // set the dimensions and margins of the graph
    let margin = histogram_config.margin;
    let width = histogram_config.width;
    let height = histogram_config.height;

    // append the svg object to the body of the page
    var svg = d3.select("#histogram")
        .append("svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom)
        .attr("style", "outline: thin solid black")
        .append("g")
        .attr("transform",
            "translate(" + margin.left + "," + margin.top + ")");

    histogram_config.svg = svg;
}

window.onload = function() {
    let margin = {top: 10, right: 30, bottom: 30, left: 40};
    histogram_config.margin = margin;
    histogram_config.height = 460 - margin.left - margin.right;
    histogram_config.width = 400 - margin.top - margin.bottom;
    init_histogram();
}

var histogram_config = {}
