/* profile_swc.h
 *  * 2015-07 : by Xiaoxiao Liu
 *   */
 
#ifndef __PROFILE_SWC_H_
#define __PROFILE_SWC_H__

#include <v3d_interface.h>

struct IMAGE_METRICS {
    double snr;
    double dy;
};

bool profile_swc_menu(V3DPluginCallback2 &callback, QWidget *parent);
bool profile_swc_func(V3DPluginCallback2 &callback, const V3DPluginArgList & input, V3DPluginArgList & output);
QList<IMAGE_METRICS> intensity_profile(NeuronTree neuronTree, Image4DSimple * image, float dilate_ratio,V3DPluginCallback2 &callback);
void printHelp(const V3DPluginCallback2 &callback, QWidget *parent);
void printHelp(const V3DPluginArgList & input, V3DPluginArgList & output);



IMAGE_METRICS compute_metrics(Image4DSimple *image,  QList<NeuronSWC> neuronSegment,float dilate_ratio, V3DPluginCallback2 &callback);
bool writeMetrics2CSV(QList<IMAGE_METRICS> result_metrics, QString output_csv_file);


#endif


