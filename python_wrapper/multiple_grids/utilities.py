import xml.etree.cElementTree as ET
import paraview
from paraview.simple import *
import logging

def rendering_multi_block_data(file_name, data_to_render):
    reader = XMLMultiBlockDataReader(FileName = file_name)
    
    show = Show(reader)
    dp = GetDisplayProperties(reader)
    dp.Representation = "Surface With Edges"
    ColorBy(dp, ("CELLS", data_to_render))
    dp.RescaleTransferFunctionToDataRange(True)
    dp.SetScalarBarVisibility(GetRenderView(), True)

    while True:
        Render()

def split_list_in_two(list_to_be_splitted):
    half_len = len(list_to_be_splitted)/2

    return list_to_be_splitted[:half_len], list_to_be_splitted[half_len:]

#def write_vtk_multi_block_data_set(**kwargs):
def write_vtk_multi_block_data_set(kwargs = {}):
    file_name = kwargs["file_name"]

    VTKFile = ET.Element("VTKFile", 
                         type = "vtkMultiBlockDataSet", 
                         version = "1.0",
                         byte_order = "LittleEndian",
                         compressor = "vtkZLibDataCompressor")

    vtkMultiBlockDataSet = ET.SubElement(VTKFile, 
                                         "vtkMultiBlockDataSet")

    iter = 0
    for pablo_file in kwargs["pablo_file_names"]:
        for vtu_file in kwargs["vtu_files"]:
            if pablo_file in vtu_file:
                DataSet = ET.SubElement(vtkMultiBlockDataSet,
                                        "DataSet",
                                        group = str(iter),
                                        dataset = "0",
                                        file = vtu_file)
                
        iter += 1

    vtkTree = ET.ElementTree(VTKFile)
    vtkTree.write(file_name)

def simple_message_log(message, logger = None):
    if logger is None:
        logger = logging.getLogger()
        logger.setLevel(logging.DEBUG)
        handler = logging.FileHandler("./Laplacian2D.log")
        formatter = logging.Formatter("%(message)s")
        handler.setFormatter(formatter)
        logger.addHandler(handler)

    logger.info(message.center(140, "-"))
    return logger

class Logger(object):
    def __init__(self, name):
        self.logger = logging.getLogger(name)
        self.logger.setLevel(logging.DEBUG)
        self.handler = logging.FileHandler("./Laplacian2D.log")

        self.formatter = logging.Formatter("%(name)15s - "    + 
                                           "%(asctime)s - "   +
                                           "%(funcName)8s - " +
                                           "%(levelname)s - " +
                                           "%(message)s")
        self.handler.setFormatter(self.formatter)
        self.logger.addHandler(self.handler)
        self.logger.propagate = False
    
    def get_logger(self):
        return self.logger