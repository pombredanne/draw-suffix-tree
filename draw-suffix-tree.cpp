#include <sdsl/suffix_trees.hpp>
#include <string>

using namespace sdsl;
using namespace std;


const char* html_header = R"content(
<!DOCTYPE html>

<html lang="en">
<head>
<meta charset="utf-8" />
<style>
.link {
  stroke: #000;
  stroke-width: 2px;
  stroke-opacity: 1;
}
</style>
</head>
<body marginwidth="0" marginheight="0">
<button><a id="download">Save as SVG</a></button>
<div class="chart"><div id="visualization"></div></div>
<script src="http://d3js.org/d3.v3.min.js"></script>
<script src="http://marvl.infotech.monash.edu/webcola/cola.v1.min.js"></script>
<script>
)content";

const char* html_body = R"content(


var w = window,d = document,e = d.documentElement,g = d.getElementsByTagName('body')[0];
var xw = w.innerWidth || e.clientWidth || g.clientWidth;
var yh = w.innerHeight || e.clientHeight || g.clientHeight;

var cola = cola.d3adaptor()
    .linkDistance(120)
    .avoidOverlaps(true)
    .flowLayout('y')
    .size([xw,yh]);

var svg = d3.select("#visualization").append("svg")
    .attr("width", xw)
    .attr("height", yh);

var g = svg.append("g");

cola
    .nodes(graph.nodes)
    .links(graph.links)
    .constraints(graph.constraints)
    .avoidOverlaps(false)
    .start(10,10,10);

var link = g.selectAll(".link")
    .data(graph.links)
    .enter().append("g")
    .attr("class", "edge")

var node = g.selectAll(".node")
    .data(graph.nodes)
    .enter().append("g")
    .attr("class", function(d) { return d.leaf ? "leaf" : "inner";});

g.selectAll(".inner").append("path")
    .attr("d", d3.svg.symbol().type("circle").size(150))
    .attr("class", "innernode")
    .attr("transform", function(d) { return "translate(" + d.x + "," + d.y + ")"; });

leaf_nodes = g.selectAll(".leaf")
leaf_text = leaf_nodes.append('text')
    .attr("transform", function(d) { return "translate(" + d.x + "," + d.y + ")"; })
    .attr("text-anchor", "middle")
    .attr("class", "leaftext")
    .style("font-family", "monospace")
    .style("font-size", "20px")
    .attr("dy", 20)
    .text(function(d){return d.suffix;});

leaf_text.each(function() {
    bbox = this.getBBox();
    d3.select(this.parentNode).append('rect')
        .attr("transform", function(d) { return "translate(" + (d.x + bbox.x - 2.5) + "," + (d.y + bbox.y-5) + ")"; })
        .attr("width", bbox.width+5)
        .attr("height", bbox.height+7)
        .style("fill", "#FF0000")
        .style("fill-opacity", ".3")
        .style("stroke", "#FF0000")
        .style("stroke-width", "1.5px");
});

node.append("title")
    .text(function (d) { return d.range; });

link_paths = link.append("line")
    .attr("class", "link")
    .attr("x1", function(d) { return d.source.x; })
    .attr("y1", function(d) { return d.source.y; })
    .attr("x2", function(d) { return d.target.x; })
    .attr("y2", function(d) { return d.target.y; });
 
 
var label_path = g.selectAll(".edge")
    .append("path")
    .attr("class", "label_path")
    .attr("id", function(d) { return d.source.id + "-" + d.target.id; })
    .attr('d', function(d) { return calc_path(d) })
 
var path_label = g.append("svg:g").selectAll(".label_path")
    .data(graph.links, function(d) { return d.source.id + "-" + d.target.id; })
    .enter().append("svg:text")
    .attr("class", "path_label")
    .append("svg:textPath")
    .attr("startOffset", "50%")
    .attr("text-anchor", "middle")
    .attr("xlink:href", function(d) { return "#" + d.source.id + "-" + d.target.id; })
    .style("fill", "#000")
    .style("font-family", "monospace")
    .style("font-size", "20px")
    .text(function(d) { return d.target.x > d.source.x ? d.target.edge : d.target.edge.split("").reverse().join("") });

// try to resize with the right size
var largest_x_leaf = d3.max(g.selectAll(".leaftext")[0], function(d) { return d3.transform(d3.select(d).attr("transform")).translate[0]; })
var largest_y_leaf = d3.max(g.selectAll(".leaftext")[0], function(d) { return d3.transform(d3.select(d).attr("transform")).translate[1]; })

var largest_x = d3.max(graph.links,function(d) { return d.target.x; })
var largest_y = d3.max(graph.links,function(d) { return d.target.y; })

g.attr("transform", "translate(" + 0 + "," + 50 + ")")
svg.attr("width",largest_x_leaf+100).attr("height",largest_y_leaf+100)

function calc_path(d) {
    var start = d.target.x > d.source.x ? d.source : d.target;
    var end = d.target.x > d.source.x ? d.target : d.source;
    var deltaX = end.x - start.x,
    deltaY = end.y - start.y,
    dist = Math.sqrt(deltaX * deltaX + deltaY * deltaY),
    normX = deltaX / dist,
    normY = deltaY / dist,
    sourcePadding = d.left ? 17 : 12,
    targetPadding = d.right ? 17 : 12,
    sourceX = start.x + (sourcePadding * normX),
    sourceY = start.y + (sourcePadding * normY),
    targetX = end.x - (targetPadding * normX),
    targetY = end.y - (targetPadding * normY);
    return 'M' + sourceX + ',' + sourceY + 'L' + targetX + ',' + targetY;
}

d3.select("#download").on("click", function () {
    d3.select(this).attr("href", 'data:application/octet-stream;base64,' 
        + btoa(d3.select("#visualization").html())).attr("download", "viz.svg")})
 
</script>
</body>

</html>
)content";

int main(int argc,char** argv)
{
    if(argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <text>" << std::endl;
        return EXIT_SUCCESS;
    }
    std::string input = argv[1];

    string file = "@cst-input";
    store_to_file(input.c_str(), file);

    // construct cst
    typedef cst_sct3<> cst_type;
    cst_type cst;
    construct(cst, file, 1 );

    std::cout << html_header;

    std::cout << "var graph = {\n\t \"nodes\": [\n";
    std::unordered_map<uint64_t,uint64_t> node_id_map;
    size_t j=0;
    for (auto it=cst.begin(); it!=cst.end(); ++it) {
        auto node = *it;
        if(it.visit()==1) {
            std::vector<cst_type::char_type> edge_label;
            if(node != cst.root()) {
                auto parent = cst.parent(node);
                auto parent_depth = cst.depth(parent);
                auto node_depth = cst.depth(node);
                auto edge_len = node_depth-parent_depth;
                if(edge_len > 20) {
                    for(size_t i=parent_depth+1;i<=parent_depth+8;i++) {
                        auto edge_sym = cst.edge(node,i);
                        edge_label.push_back(edge_sym);
                    }
                    edge_label.push_back('.');
                    edge_label.push_back('.');
                    edge_label.push_back('.');
                    for(size_t i=node_depth-8;i<=node_depth;i++) {
                        auto edge_sym = cst.edge(node,i);
                        edge_label.push_back(edge_sym);
                    }
                } else {
                    for(size_t i=parent_depth+1;i<=node_depth;i++) {
                        auto edge_sym = cst.edge(node,i);
                        edge_label.push_back(edge_sym);
                    }
                }
            }
            if( cst.is_leaf(node) ) {
                std::cout << "\t\t{\"id\": " << cst.id(node) << ","
                          << " \"leaf\":1, \"suffix\":" << cst.sn(node) << ","
                          << " \"edge\": \"";
                for(const auto& es : edge_label) {
                    if(es!=0&&es!=1) std::cout << es;
                    if(es==0) std::cout << '$';
                    if(es==1) std::cout << '#';
                }
                std::cout << "\"}";
            } else {
                std::cout << "\t\t{\"id\": " << cst.id(node) << ","
                          << " \"range\": \"[" << cst.lb(node) << "," << cst.rb(node) << "]\","
                          << " \"edge\": \"";
                for(const auto& es : edge_label) {
                    if(es!=0&&es!=1) std::cout << es;
                    if(es==0) std::cout << '$';
                    if(es==1) std::cout << '#';
                }
                std::cout << "\"}";
            }
            node_id_map[cst.id(node)] = j;

            if(node_id_map.size() == cst.nodes()) {
                std::cout << "\n";
            } else {
                std::cout << ",\n";
            }

            j++;

        }
    }
    std::cout << "\t],\n";
    std::cout << "\t \"links\": [\n";
    size_t num_visisted = 0;
    for (auto it=cst.begin(); it!=cst.end(); ++it) {
        auto node = *it;
        if(it.visit()==1) {
            if(node != cst.root()) {
                auto parent = cst.parent(node);
                auto parent_id = node_id_map[cst.id(parent)];
                auto node_id = node_id_map[cst.id(node)];
                std::cout << "\t\t{\"source\":" << parent_id << ", \"target\": " << node_id << "}";
                num_visisted++;
                if(num_visisted == cst.nodes()-1) {
                    std::cout << "\n";
                } else {
                    std::cout << ",\n";
                }   
            }
        }
    }
    std::cout << "\t],\n";
    std::cout << "\t \"constraints\": [\n";

    /* constraint on leaf positions */
    struct constraint {
        uint8_t axis;
        uint64_t left_id;
        uint64_t right_id;
        uint64_t gap;
        constraint(uint8_t a,uint64_t l,uint64_t r,uint64_t g) : axis(a), left_id(l), right_id(r), gap(g) {}
    };
    std::vector<constraint> constraints;
    for(size_t i=2;i<=cst.size();i++) {
        auto left_leaf = cst.select_leaf(i-1);
        auto right_leaf = cst.select_leaf(i);
        auto left_parent = cst.parent(left_leaf);
        auto right_parent = cst.parent(right_leaf);
        auto left_id = node_id_map[cst.id(left_leaf)];
        auto right_id = node_id_map[cst.id(right_leaf)];
        if(left_parent == right_parent) {
            constraints.emplace_back('x',left_id,right_id,90);
        } else {
            constraints.emplace_back('x',left_id,right_id,40);
        }
    }

    /* constraint on height */
    for (auto it=cst.begin(); it!=cst.end(); ++it) {
        auto node = *it;
        auto node_id = node_id_map[cst.id(node)];
        auto node_depth = cst.depth(node);
        auto degree = cst.degree(node);
        if(it.visit()==2 && degree > 1) {
            for(auto& child: cst.children(node)) {
                auto child_id = node_id_map[cst.id(child)];
                auto child_depth = cst.depth(child);
                auto edge_len = child_depth - node_depth;
                if(edge_len > 20) edge_len = 20;
                constraints.emplace_back('y',node_id,child_id,30+edge_len*15);
            }
        }
    }

    /* constraint on level  */
    for (auto it=cst.begin(); it!=cst.end(); ++it) {
        auto node = *it;
        auto node_id = node_id_map[cst.id(node)];
        auto node_depth = cst.depth(node);
        auto degree = cst.degree(node);
        if(it.visit()==1 && degree > 1) {
            for(size_t i=2;i<=degree;i++) {
                auto first_child = cst.select_child(node,i-1);
                auto second_child = cst.select_child(node,i);
                auto first_id = node_id_map[cst.id(first_child)];
                auto second_id = node_id_map[cst.id(second_child)];
                constraints.emplace_back('x',first_id,second_id,25);
            }
        }
    }

    /* more leaf constraints */
    for(size_t i=1;i<=cst.size();i++) {
        auto leaf = cst.select_leaf(i);
        auto leaf_id = node_id_map[cst.id(leaf)];
        auto parent = cst.parent(leaf);
        auto sibling = cst.sibling(parent);
        if( sibling != cst.root()) {
            auto sibling_id = node_id_map[cst.id(sibling)];
            constraints.emplace_back('x',leaf_id,sibling_id,20);
        }
    }

    /* constraint on children with differnet edge lengths  */
    for (auto it=cst.begin(); it!=cst.end(); ++it) {
        auto node = *it;
        auto node_id = node_id_map[cst.id(node)];
        auto node_depth = cst.depth(node);
        auto degree = cst.degree(node);
        if(it.visit()==2 && degree > 1) {
            for(auto& child: cst.children(node)) {
                auto child_id = node_id_map[cst.id(child)];
                auto child_depth = cst.depth(child);
                auto edge_len = child_depth - node_depth;
                auto sibling = cst.sibling(child);
                if( sibling != cst.root() ) {
                    auto sibling_depth = cst.depth(sibling);
                    auto sibling_edge_len = sibling_depth - node_depth;
                    if( abs(((int)sibling_edge_len - (int)edge_len)) > 5 ) {
                        auto sibling_id = node_id_map[cst.id(sibling)];
                        constraints.emplace_back('y',sibling_id,child_id,10);                    } 
                }
            }
        }
    }

    /* write the constraints */
    for(size_t i=0;i<constraints.size()-1;i++) {
        const auto& c = constraints[i];
        std::cout << "\t\t{ \"axis\": \"" << c.axis << "\", \"left\": " << c.left_id
                  << ", \"right\": " << c.right_id << ", \"gap\": " << c.gap << "},\n";
    }
    const auto& c = constraints.back();
    std::cout << "\t\t{ \"axis\": \"" << c.axis << "\", \"left\": " << c.left_id
              << ", \"right\": " << c.right_id << ", \"gap\": " << c.gap << "}\n";

    std::cout << "\t ]\n";
    std::cout << "}\n";

    std::cout << html_body;

}
