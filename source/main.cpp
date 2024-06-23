#include <iostream>
#include "load_onnx.h"
#include <string>
int main()
{

    std::cout << "hello" << std::endl;

    std::string device = "cpu";
    std::vector<std::vector<int64_t>> input_shapes;
    std::vector<std::string> input_types;
    std::vector<std::vector<int64_t>> input_shapes2;
    std::vector<std::string> input_types2;
    std::vector<std::string> customop_modules;
    std::vector<std::string> module_operators;
    pnnx::Graph pnnx_graph;
    std::string ptpath = "/home/faith/ONNX-YOLOv8-Object-Detection/yolov8n.onnx";
    pnnx::load_onnx(ptpath.c_str(), pnnx_graph,
              input_shapes, input_types,
              input_shapes2, input_types2);
}