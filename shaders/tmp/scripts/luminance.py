from reflection import *

NUM_HISTOGRAM_BINS = 256

Params = [
    StructField(TYPE_FLOAT, "HistogramLogMin"),
    StructField(TYPE_FLOAT, "HistogramLogMax"),
    StructField(TYPE_FLOAT, "SpeedUp"),
    StructField(TYPE_FLOAT, "SpeedDown"),
    StructField(TYPE_FLOAT, "ExposureCompensation"),
    StructField(TYPE_FLOAT, "Mode"),
    StructField(TYPE_FLOAT, "DeltaTime"),
    StructField(TYPE_FLOAT, "TotalPixelsCount"),
]

Histogram = [
    StructField(TYPE_UINT, "Bins", ArrayQualifier(NUM_HISTOGRAM_BINS)),
]

Luminance = [
    StructField(TYPE_FLOAT, "LumTemporal"),
    StructField(TYPE_FLOAT, "AutoExposure"),
]
