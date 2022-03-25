from pathlib import Path
import dipy.tracking.streamline as dts
import numpy as np
from dipy.segment.clustering import QuickBundles
import dipy.segment.metric as dsm
import dipy.tracking.streamline as dts
import nibabel.streamlines as nibstream

from intersection.cortical_intersections import Bundle


def get_vtk_bundles(folder: Path):
    bundles = [nibstream.load(path).streamlines for path in folder.glob("*.tck")]
    for bundle in bundles:
        orient_fibers(bundle)
    return [
        Bundle([fiber for fiber in bundle], str(path))
        for bundle, path in zip(bundles, folder.glob("*.tck"))
        if len(bundle)
    ]


def orient_fibers(streamlines: nibstream.ArraySequence):
    if len(streamlines):
        feature = dsm.ResampleFeature(nb_points=100)
        metric = dsm.AveragePointwiseEuclideanMetric(feature)
        qb = QuickBundles(np.inf, metric=metric)
        cluster = qb.cluster(streamlines)
        centroid = cluster.centroids[0]
        dts.orient_by_streamline(streamlines, centroid, in_place=True)
