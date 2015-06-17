/* RegressionTubularityAC_plugin.cpp
 * a plugin for test
 * 2015-5-5 : by HP
 */

#define myQDebug(...) printf (__VA_ARGS__)
#define myQFatal(...) do{ printf (__VA_ARGS__); exit(1); } while(0)

#include "v3d_message.h"
#include <vector>
#include "SQBTree_plugin.h"
#include "sqb_trees.h"
#include <math.h>

#include "sqb_0.1/src/MatrixSQB/vaa3d_link.h"

#include "regression/sep_conv.h"
#include "regression/sampling.h"
#include "regression/regressor.h"
//#include "regression/regression_test2.h"

#include <sys/stat.h>
#include <sys/types.h>
//#include <boost/filesystem.hpp>
//#include <SQB/Core/RegTree.h>
//#include <SQB/Core/Utils.h>

//#include <SQB/Core/Booster.h>

//#include <SQB/Core/LineSearch.h>



//using namespace Eigen;
typedef Eigen::VectorXf VectorTypeFloat;
typedef Eigen::VectorXd VectorTypeDouble;
typedef Eigen::MatrixXd MatrixTypeDouble;
typedef Eigen::MatrixXf MatrixTypeFloat;


typedef SQB::TreeBoosterWeightsType  WeightsType;
typedef float   FeatsType;

typedef Eigen::Array<FeatsType, Eigen::Dynamic, Eigen::Dynamic>     gFeatArrayType;
typedef Eigen::Array<WeightsType, Eigen::Dynamic, 1>                gResponseArrayType;
typedef Eigen::Array<WeightsType, Eigen::Dynamic, 1>                gWeightsArrayType;


typedef float ImageScalarTypeFloat;
typedef unsigned char ImageScalarTypeUint8;
typedef itk::Image< ImageScalarTypeFloat, 3 >         ITKImageType;


typedef SQB::MatrixFeatureIndexList<gFeatArrayType>          MatrixFeatureIndexListType;
typedef SQB::MatrixSampleIndexList<gFeatArrayType>           MatrixSampleIndexListType;
typedef SQB::MatrixFeatureValueObject<gFeatArrayType>        MatrixFeatureValueObjectType;
typedef SQB::MatrixSingleResponseValueObject<gResponseArrayType>   MatrixClassifResponseValueObjectType;


typedef SQB::TreeBooster<
            MatrixSampleIndexListType,
            MatrixFeatureIndexListType,
            MatrixFeatureValueObjectType,
            MatrixClassifResponseValueObjectType >      TreeBoosterType;

using namespace std;
Q_EXPORT_PLUGIN2(SQBTree, SQBTreePlugin);


bool trainTubularityImage(V3DPluginCallback2 &callback, const V3DPluginArgList & input, V3DPluginArgList & output);

bool testTubularityImage(V3DPluginCallback2 &callback, const V3DPluginArgList & input, V3DPluginArgList & output);

bool resampleImage(V3DPluginCallback2 &callback, const V3DPluginArgList & input, V3DPluginArgList & output);

void convolveV3D(unsigned char *data1d,V3DLONG *in_sz,float* &outimg, const unsigned int unit_bites,V3DPluginCallback2 &callback);

QStringList SQBTreePlugin::menulist() const
{
  return QStringList()
      <<tr("menu1")
     <<tr("menu2")
    <<tr("about");
}

QStringList SQBTreePlugin::funclist() const
{
  return QStringList()
      <<tr("func1")
     <<tr("func2")
    <<tr("help");
}

void SQBTreePlugin::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
  if (menu_name == tr("menu1"))
  {
    v3d_msg("To be implemented.");
  }
  else if (menu_name == tr("menu2"))
  {
    v3d_msg("To be implemented.");
  }
  else
  {
    v3d_msg(tr("a plugin for test. "
               "Developed by HP, 2015-5-5"));
  }
}

bool SQBTreePlugin::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
{
  //	vector<char*> infiles, inparas, outfiles;
  //	if(input.size() >= 1) infiles = *((vector<char*> *)input.at(0).p);
  //	if(input.size() >= 2) inparas = *((vector<char*> *)input.at(1).p);
  //	if(output.size() >= 1) outfiles = *((vector<char*> *)output.at(0).p);

  if(func_name == tr("train"))
  {
      return trainTubularityImage(callback, input, output);

  }
  else if (func_name == tr("test")) // apply already trained classifier to an image and save results
  {

    return testTubularityImage(callback, input, output);

  }
  else if (func_name == tr("resample"))
  {
    return resampleImage(callback, input, output);
  }
  else if (func_name == tr("help"))
  {
    v3d_msg("To be implemented.");
  }
  else return false;

  return true;
}


bool trainTubularityImage(V3DPluginCallback2 &callback, const V3DPluginArgList & input, V3DPluginArgList & output)
{

 //sample call:  ./vaa3d -x RegressionTubularityAC -f train -i /cvlabdata1/cvlab/datasets_amos/data3D/OPF/train/images/images_tif/OP_1.tif -o /cvlabdata1/home/asironi/vaa3d/vaa3d_tools/bigneuron_ported/AmosSironi_PrzemyslawGlowacki/SQBTree_plugin/regression/OPF1_bf_filters_ac -p /cvlabdata1/cvlab/datasets_amos/data3D/OPF/train/radial_gt/images_tif/OP_1_radial_gt.tif 1 2000 2000 15 0 0.1 50

     cout<<"Reading Input Files and Parameters."<<endl;
    if (output.size() != 1) return false;
     char * regressor_output_dir = ((vector<char*> *)(output.at(0).p))->at(0);

     int created_dir = mkdir(regressor_output_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

     if(created_dir<0){
         std::cout << "Could not create output directory (directory may already exist): " << regressor_output_dir << endl;
                    return false;

     }
     unsigned int str_length = (unsigned)strlen(regressor_output_dir);
    vector<char*> *trainImagePaths = ((vector<char*> *)(input.at(0).p)); //train images

    unsigned int n_train_images = trainImagePaths->size(); //

    if (n_train_images < 1){

        cout<<"No input train images!"<<endl;
        return false;
    }

    //first n_train_images parameters are swc files
    vector<char*> *parameters = ((vector<char*> *)(input.at(1).p));
        if(parameters->size() < n_train_images){
            cout<<"Number of input train images and swc files must be the same !"<<endl;
            return false;
        }

        //get swc filenames
        vector<char*> trainGtSwcPaths(n_train_images);
        for(unsigned int i_swc = 0; i_swc<n_train_images; i_swc++){
            trainGtSwcPaths.at(i_swc) =  parameters->at(i_swc);
        }

        ////Auto-Context params
        unsigned int n_ac_iters = 1;
        if(parameters->size()>n_train_images){
            n_ac_iters = atoi(parameters->at(n_train_images));
        }
        unsigned int n_pos_samples_tot =50000;
        unsigned int n_neg_samples_tot =50000;
        unsigned int n_samples_tot =n_pos_samples_tot+ n_neg_samples_tot;


        if(parameters->size()>n_train_images+1){
            n_samples_tot= atoi(parameters->at(n_train_images+1));
            n_samples_tot = 2*(n_samples_tot/2);//ensure it is even
            n_pos_samples_tot = n_samples_tot/2;
            n_neg_samples_tot = n_samples_tot/2;
        }

    ////boosting parameters
     unsigned max_boost_iters = 200;
     unsigned max_depth_wl_tree = 2;
     double shrink_factor = 0.1;
    unsigned int m_try =0;
    if(parameters->size()>n_train_images+2){
        max_boost_iters = atoi(parameters->at(n_train_images+2));
    }
    if(parameters->size()>n_train_images+3){
        max_depth_wl_tree = atoi(parameters->at(n_train_images+3));
    }
    if(parameters->size()>n_train_images+4){
        shrink_factor = atof(parameters->at(n_train_images+4));
    }if(parameters->size()>n_train_images+5){
        m_try = atoi(parameters->at(n_train_images+5));
    }
    char * loss_type = "squaredloss";


    ////get filters to compute features
 //image features
   const char *weight_file_im ="/cvlabdata1/cvlab/datasets_amos/data3D/filter_banks/oof/oof_fb_3d_scale_1_2_3_5_8_size_21_weigths_cpd_rank_49.txt";
       const char *sep_filters_file_im ="/cvlabdata1/cvlab/datasets_amos/data3D/filter_banks/oof/oof_fb_3d_scale_1_2_3_5_8_size_21_sep_cpd_rank_49.txt";
    //autocontext features
       const char *weight_file_ac = "/cvlabdata1/cvlab/datasets_amos/data3D/filter_banks/oof/proto_filter_AC_lap_633_822_weigths_cpd_rank_49.txt";
       const char *sep_filters_file_ac = "/cvlabdata1/cvlab/datasets_amos/data3D/filter_banks/oof/proto_filter_AC_lap_633_822_sep_cpd_rank_49.txt";


    MatrixTypeDouble weights = readMatrix(weight_file_im);
    MatrixTypeDouble sep_filters = readMatrix(sep_filters_file_im);
    MatrixTypeFloat sep_filters_float = sep_filters.cast<float>();
    MatrixTypeFloat weights_float = weights.cast<float>();


    MatrixTypeDouble weights_ac = readMatrix(weight_file_ac); //weights_ac = weights_ac/255.0;
    MatrixTypeDouble sep_filters_ac = readMatrix(sep_filters_file_ac);
    MatrixTypeFloat sep_filters_float_ac = sep_filters_ac.cast<float>();
    MatrixTypeFloat weights_float_ac = weights_ac.cast<float>();

    unsigned int n_features_tot = weights_float.cols();//for now just number of non-sep filters (TODO: add context features)
    const float scale_factor =1.0; //no rescale filter
    unsigned int channel = 1; //only process first channel of image

    if(m_try>weights_float.cols()){
        std:cout << "m_try larger than number of features. Using all features" << std::endl;
            m_try  = weights_float.cols();
    }

    MatrixTypeFloat * features_image = new MatrixTypeFloat[n_train_images];
    VectorTypeFloat * train_gt_vector = new VectorTypeFloat[n_train_images];
    ITKImageType::SizeType * size_img = new ITKImageType::SizeType[n_train_images];
        // cout<<"Computing Features!"<<endl;
//        //// get gt, compute features and random samples
//        unsigned int n_pos_samples_per_image;
//        unsigned int n_neg_samples_per_image;
//        unsigned int collected_pos_samples = 0;
//        unsigned int collected_neg_samples = 0;

//        MatrixTypeFloat all_samples_features = MatrixTypeFloat::Zero(n_samples_tot,n_features_tot) ;
//        VectorTypeFloat all_samples_gt = VectorTypeFloat::Zero(n_samples_tot) ;

//        MatrixTypeFloat sampled_features_image;
//        VectorTypeFloat sampled_gt_vector_image;
        for(unsigned int i_img =0; i_img<n_train_images; i_img++){

            cout<<"Processing Train Image "<< i_img+1<< "/"<< n_train_images << endl;

//            if(i_img<n_train_images-1)
//            {
//                n_pos_samples_per_image = n_pos_samples_tot/n_train_images;
//                n_neg_samples_per_image = n_neg_samples_tot/n_train_images;
//            }
//            else
//            {
//                n_pos_samples_per_image = n_pos_samples_tot - collected_pos_samples;
//                n_neg_samples_per_image = n_neg_samples_tot - collected_neg_samples;
//            }


            char * train_img_file = trainImagePaths->at(i_img);
            cout<<"image file "<< train_img_file << endl;

            char * swc_gt_file = trainGtSwcPaths.at(i_img);
            cout<<"swc file "<< swc_gt_file << endl;

            Image4DSimple *train_img = callback.loadImage(train_img_file);
            if (!train_img || !train_img->valid())
            {
              v3d_msg("Fail to open the image file.", 0);
              return false;
            }

            V3DLONG train_img_size[4];
            train_img_size[0] = train_img->getXDim();
            train_img_size[1] = train_img->getYDim();
            train_img_size[2] = train_img->getZDim();
            train_img_size[3] = train_img->getCDim();

            V3DLONG n_pixels = train_img_size[0]*train_img_size[1]*train_img_size[2];

            size_img[i_img][0] =train_img_size[0];
            size_img[i_img][1] =train_img_size[1];
            size_img[i_img][2] =train_img_size[2];

            ImagePixelType pixel_type = train_img->getDatatype();

            ////convert image to itk format
            ITKImageType::Pointer train_img_ITK  =  ITKImageType::New();

            train_img_ITK =v3d2ItkImage<ITKImageType,ImageScalarTypeUint8>(train_img,train_img_size,channel);

            ////TODO ! convert swc file to distance gt image

            ITKImageType::Pointer train_gt_radial_ITK  =  ITKImageType::New();
            train_gt_radial_ITK =swc2ItkImage<ITKImageType,ImageScalarTypeFloat>(swc_gt_file,train_img_size);//for now return null pointer !

            // Test! To be deleted!!!  // // // // // // //
            std::cout << "Attempting to convert to V3D! size: " << train_gt_radial_ITK->GetLargestPossibleRegion().GetSize() << std::endl << std::flush;

            typename ITKImageType::IndexType pixelIndex; pixelIndex[0] = pixelIndex[1] = pixelIndex[2] = 0;
            std::cout << "First pixel: " << train_gt_radial_ITK->GetPixel(pixelIndex) << std::endl << std::flush;

            Image4DSimple IinV3dFormat = itk2v3dImage<ITKImageType>(train_gt_radial_ITK);
            std::cout << "Attempting to save to file!\n" << std::flush;
            callback.saveImage(&IinV3dFormat, "/cvlabdata1/home/pglowack/Data/3dtest/geneva/inputs/3d/test/images_respaced/images_respaced-tif/whatever.v3draw");














//            //copy data to save with v3d format


//            V3DLONG n_pixelsss = train_gt_radial_ITK->GetLargestPossibleRegion().GetSize()[0]
//                             * train_gt_radial_ITK->GetLargestPossibleRegion().GetSize()[1]
//                             * train_gt_radial_ITK->GetLargestPossibleRegion().GetSize()[2];
//           float* out_data_copyyy = new float[n_pixelsss];
//         for(unsigned int i_pix = 0; i_pix < n_pixelsss; i_pix++){
//                out_data_copyyy[i_pix] = (float) (i_pix);
//           }


//     outimg1.setData((unsigned char *)(out_data_copy), size_img_scaled[0], size_img_scaled[1], size_img_scaled[2], 1, V3D_FLOAT32);



//            char outimg_file_ac [500];

//                sprintf (outimg_file_ac, "/cvlabdata1/home/asironi/vaa3d/vaa3d_tools/bigneuron_ported/AmosSironi_PrzemyslawGlowacki/SQBTree_plugin/regression/temp_results/score_img_%i_ac_%i_scale_%i.v3draw",imgInd,i_ac,i_scale);

//                cout << "saving: " << outimg_file_ac<<endl;
//              callback.saveImage(&outimg1, outimg_file_ac);










            // // // // // // // // // // // // // // // //


    //// //for now load radial gt and compute exp dist gt
             cout<<"gt file "<< swc_gt_file << endl;
             Image4DSimple *train_radial_gt = callback.loadImage(swc_gt_file);
             if (!train_radial_gt || !train_radial_gt->valid())
             {
               v3d_msg("Fail to open the radial gt image file.", 0);
               return false;
             }
              train_gt_radial_ITK = v3d2ItkImage<ITKImageType,ImageScalarTypeUint8>(train_radial_gt,train_img_size,channel);

              cout << "Computing distance gt...  "<< endl;
            // train_gt_ITK = radialGt2ExpDistGt<ITKImageType>(train_gt_radial_ITK,thresh_distance,scales,scale_toll);
             float thresh_distance = 21.0;
             ITKImageType::Pointer  train_gt_ITK_computed = binaryGt2ExpDistGt<ITKImageType>(train_gt_radial_ITK,thresh_distance);
             cout << "Computing distance gt... Done."<< endl;

           train_gt_vector[i_img]  = itkImage2EigenVector<ITKImageType,VectorTypeFloat>(train_gt_ITK_computed,n_pixels,n_pixels);

            ////compute features image (TODO add context features)
            cout<<"Computing Features..."<<endl;
            computeFeaturesSepComb<ITKImageType,MatrixTypeFloat,VectorTypeFloat>(features_image[i_img],train_img_ITK,sep_filters_float,weights_float, scale_factor);

            cout<<"Computing Features...Done!"<<endl;

 //   cout<<"Rows Features: "<<features_image[i_img].rows()<<" Cols Features: "<<features_image[i_img].cols() <<endl;
    //cout<< "n pixels: "<< n_pixels << endl;


//            ////random sampling
//            cout<<"Getting Samples..."<<endl;
//            bool got_samples = getTrainSamplesFeaturesAndGt<MatrixTypeFloat,VectorTypeFloat>(features_image,train_gt_vector,sampled_features_image, sampled_gt_vector_image,n_pos_samples_per_image,n_neg_samples_per_image);

//            if(!got_samples){
//                cout<< "Problems encountered while getting samples ! " << endl;
//                return false;
//            }
//    cout<<"Getting Samples...Done!"<<endl;


//            all_samples_features.middleRows(i_img*(collected_pos_samples+collected_neg_samples), n_pos_samples_per_image+n_neg_samples_per_image ) = sampled_features_image;//P.middleRows(i, rows) <=> P(i+1:i+rows, :)
//            all_samples_gt.middleRows(i_img*(collected_pos_samples+collected_neg_samples), n_pos_samples_per_image+n_neg_samples_per_image ) = sampled_gt_vector_image;

//            collected_pos_samples += n_pos_samples_per_image;
//            collected_neg_samples += n_neg_samples_per_image;
        }

        cout<<"Processed all Train Images "<< endl;

  cout<<"Starting Training..."<<endl;
        //Auto-Context iterations
  MatrixTypeFloat features_image_and_ac;
  MatrixTypeFloat *features_ac = new MatrixTypeFloat[n_train_images];
    for(unsigned int i_ac=0; i_ac <= n_ac_iters; i_ac++){
        cout<<"Auto-Context Iter "<< i_ac<< "/" <<n_ac_iters<< endl;


        if(i_ac>0){
            n_features_tot = features_image[0].cols()+features_ac[0].cols();
        } //for now use same filter for image and autocontext features

      //  cout << "n features all: " <<n_features_tot << endl;

        // get samples
        ////random sampling
        ///  //// get gt, compute features and random samples
        unsigned int n_pos_samples_per_image;
        unsigned int n_neg_samples_per_image;
        unsigned int collected_pos_samples = 0;
        unsigned int collected_neg_samples = 0;

        MatrixTypeFloat all_samples_features = MatrixTypeFloat::Zero(n_samples_tot,n_features_tot) ;
        VectorTypeFloat all_samples_gt = VectorTypeFloat::Zero(n_samples_tot) ;

        MatrixTypeFloat sampled_features_image;
        VectorTypeFloat sampled_gt_vector_image;
        ///
        cout<<"Getting Samples..."<<endl;

        for(unsigned int i_img =0; i_img<n_train_images; i_img++){



                if(i_ac>0){

                    features_image_and_ac =  MatrixTypeFloat::Zero(features_image[i_img].rows(),n_features_tot) ;
                    features_image_and_ac.middleCols(0,features_image[0].cols()) = features_image[i_img];
                    features_image_and_ac.middleCols(features_image[0].cols(),features_ac[0].cols()) = features_ac[i_img];

                }else{
                    features_image_and_ac = features_image[i_img];
                }
           //     cout << "size feats all: " <<features_image_and_ac.rows()<<" "<<features_image_and_ac.cols() << endl;


                if(i_img<n_train_images-1)
                {
                    n_pos_samples_per_image = n_pos_samples_tot/n_train_images;
                    n_neg_samples_per_image = n_neg_samples_tot/n_train_images;
                }
                else
                {
                    n_pos_samples_per_image = n_pos_samples_tot - collected_pos_samples;
                    n_neg_samples_per_image = n_neg_samples_tot - collected_neg_samples;
                }

                bool got_samples = getTrainSamplesFeaturesAndGt<MatrixTypeFloat,VectorTypeFloat>(features_image_and_ac,train_gt_vector[i_img],sampled_features_image, sampled_gt_vector_image,n_pos_samples_per_image,n_neg_samples_per_image);

                if(!got_samples){
                    cout<< "Problems encountered while getting samples ! " << endl;
                    return false;
                }

                all_samples_features.middleRows((collected_pos_samples+collected_neg_samples), n_pos_samples_per_image+n_neg_samples_per_image ) = sampled_features_image;//P.middleRows(i, rows) <=> P(i+1:i+rows, :)
                all_samples_gt.middleRows((collected_pos_samples+collected_neg_samples), n_pos_samples_per_image+n_neg_samples_per_image ) = sampled_gt_vector_image;


                collected_pos_samples += n_pos_samples_per_image;
                collected_neg_samples += n_neg_samples_per_image;

        }//end sampling image
         cout<<"Getting Samples...Done!"<<endl;
         cout<<"Collected " << collected_pos_samples << " Pos samples and " <<collected_neg_samples << " negative samples."<< endl;

//    cout<<"Max features: "<< all_samples_features.cast<float>().maxCoeff() <<"Min features: "<< all_samples_features.cast<float>().minCoeff()  << endl;
//    cout<<"Max gt: "<< all_samples_gt.cast<double>().maxCoeff() <<"Min gt: "<< all_samples_gt.cast<double>().minCoeff()  << endl;


//        if(i_ac>0){
//cout<<"writing matrix ac1 :" << endl;
//            writeMatrix<MatrixTypeFloat>("/cvlabdata1/home/asironi/vaa3d/vaa3d_tools/bigneuron_ported/AmosSironi_PrzemyslawGlowacki/SQBTree_plugin/regression/results/feats_AC1.txt",all_samples_features.cast<float>());
//            writeMatrix<MatrixTypeDouble>("/cvlabdata1/home/asironi/vaa3d/vaa3d_tools/bigneuron_ported/AmosSironi_PrzemyslawGlowacki/SQBTree_plugin/regression/results/gt_AC1.txt",all_samples_gt.cast<double>());

//        }else{

//             cout<<"writing matrix :" << endl;
//            writeMatrix<MatrixTypeFloat>("/cvlabdata1/home/asironi/vaa3d/vaa3d_tools/bigneuron_ported/AmosSironi_PrzemyslawGlowacki/SQBTree_plugin/regression/results/feats_AC0.txt",all_samples_features.cast<float>());
//            writeMatrix<MatrixTypeDouble>("/cvlabdata1/home/asironi/vaa3d/vaa3d_tools/bigneuron_ported/AmosSironi_PrzemyslawGlowacki/SQBTree_plugin/regression/results/gt_AC0.txt",all_samples_gt.cast<double>());

//        }


    char regressor_output_file[str_length+50];
    sprintf (regressor_output_file, "%s/Regressor_ac_%i.cfg",regressor_output_dir,i_ac);

////Training
        trainRegressor(all_samples_features.cast<float>(),all_samples_gt.cast<double>(),regressor_output_file,loss_type,max_boost_iters,max_depth_wl_tree,shrink_factor,m_try);


        if(i_ac<n_ac_iters && n_ac_iters>0){

            std::cout << "Computing Features Auto-Context..." << std::endl;
            for(unsigned int i_img =0; i_img<n_train_images; i_img++){

                cout<<"...Train Image "<< i_img+1<< "/"<< n_train_images << "..."<<endl;


                if(i_ac>0){

                    //features_image_and_ac << features_image[i_img], features_ac[i_img];
                    features_image_and_ac =  MatrixTypeFloat::Zero(features_image[i_img].rows(),n_features_tot) ;
                    features_image_and_ac.middleCols(0,features_image[0].cols()) = features_image[i_img];
                    features_image_and_ac.middleCols(features_image[0].cols(),features_ac[0].cols()) = features_ac[i_img];

                }else{
                    features_image_and_ac = features_image[i_img];
                }

                //predict
                TreeBoosterType::ResponseArrayType predScore;
                predictRegressor(regressor_output_file,features_image_and_ac,predScore);

                VectorTypeFloat predScoreFloat = predScore.cast<float>();
                ITKImageType::Pointer predImg = eigenVector2itkImage<ITKImageType,VectorTypeFloat>(predScoreFloat,size_img[i_img]);

                //compute features autocontext
                computeFeaturesSepComb<ITKImageType,MatrixTypeFloat,VectorTypeFloat>(features_ac[i_img],predImg,sep_filters_float_ac,weights_float_ac, scale_factor);

//                if(i_ac>0){
//        cout<<"writing matrix ac1 :" << endl;
//                    writeMatrix<MatrixTypeFloat>("/cvlabdata1/home/asironi/vaa3d/vaa3d_tools/bigneuron_ported/AmosSironi_PrzemyslawGlowacki/SQBTree_plugin/regression/results/feats_AC1_all.txt",features_image_and_ac.cast<float>());
//                //    writeMatrix<MatrixTypeDouble>("/cvlabdata1/home/asironi/vaa3d/vaa3d_tools/bigneuron_ported/AmosSironi_PrzemyslawGlowacki/SQBTree_plugin/regression/results/gt_AC1.txt",all_samples_gt.cast<double>());

//                }else{

//                     cout<<"writing matrix :" << endl;
//                    writeMatrix<MatrixTypeFloat>("/cvlabdata1/home/asironi/vaa3d/vaa3d_tools/bigneuron_ported/AmosSironi_PrzemyslawGlowacki/SQBTree_plugin/regression/results/feats_AC0_all.txt",features_image_and_ac.cast<float>());
//                    //writeMatrix<MatrixTypeDouble>("/cvlabdata1/home/asironi/vaa3d/vaa3d_tools/bigneuron_ported/AmosSironi_PrzemyslawGlowacki/SQBTree_plugin/regression/results/gt_AC0.txt",all_samples_gt.cast<double>());

//                }


            }

            std::cout << "Computing Features Auto-Context...Done." << std::endl;

        }

    }//end AC iters
cout<<"Training Done!"<<endl;

    cout<<"Training Terminated Normally"<<endl;
    return true;
}




bool testTubularityImage(V3DPluginCallback2 &callback, const V3DPluginArgList & input, V3DPluginArgList & output)
{

   //sample call:  ./vaa3d -x RegressionTubularityAC -f test -i /cvlabdata1/cvlab/datasets_amos/data3D/OPF/train/images/images_tif/OP_9.tif -o /cvlabdata1/home/asironi/vaa3d/vaa3d_tools/bigneuron_ported/AmosSironi_PrzemyslawGlowacki/SQBTree_plugin/regression/OPF1_bf_filters_ac/OP_9_tubularity.v3draw -p /cvlabdata1/home/asironi/vaa3d/vaa3d_tools/bigneuron_ported/AmosSironi_PrzemyslawGlowacki/SQBTree_plugin/regression/OPF1_bf_filters_ac/Regressor_ac_0.cfg /cvlabdata1/home/asironi/vaa3d/vaa3d_tools/bigneuron_ported/AmosSironi_PrzemyslawGlowacki/SQBTree_plugin/regression/OPF1_bf_filters_ac/Regressor_ac_1.cfg

  if (output.size() != 1) return false;
  const char *regressor_filename;

    vector<char*> paras = (*(vector<char*> *)(input.at(1).p));
        if(paras.size() < 1) return false;

  vector<char*> *inputImagePaths = ((vector<char*> *)(input.at(0).p));
  vector<char*> *outputImagePaths = ((vector<char*> *)(output.at(0).p));

  if(inputImagePaths->size() != outputImagePaths->size()) {
    std::cerr << "The number of input images is different than the number of output images." << std::endl;
    return false;
  } else {
   // std::cout << "inputImagePaths->size():  " << inputImagePaths->size() << std::endl
     //         << "outputImagePaths->size(): " << outputImagePaths->size() << std::endl;
  }

  unsigned int n_ac_iters = paras.size()-1;
  bool multiscale_pred = false; //todo: pass as arguments
//  bool multiscale_pred = true; //todo: pass as arguments


  for(unsigned int imgInd = 0; imgInd < inputImagePaths->size(); imgInd++) {
    char * inimg_file = inputImagePaths->at(imgInd);
    char * outimg_file = outputImagePaths->at(imgInd);

    cout<<"Predicting Test Image "<< imgInd+1<< "/"<< inputImagePaths->size() << "..."<<endl;
    cout<<"input file: "<<inimg_file<<endl;
    cout<<"output file: "<<outimg_file<<endl;


    Image4DSimple *inimg = callback.loadImage(inimg_file);
    if (!inimg || !inimg->valid())
    {
      v3d_msg("Fail to open the image file.", 0);
      return false;
    }

   // cout<<"Loaded image"<<endl;

    //input
    float* outimg = 0; //no need to delete it later as the Image4DSimple variable "outimg" will do the job

    V3DLONG in_sz[4];
    in_sz[0] = inimg->getXDim();
    in_sz[1] = inimg->getYDim();
    in_sz[2] = inimg->getZDim();
    in_sz[3] = inimg->getCDim();
    ITKImageType::SizeType  size_img;

    size_img[0] = in_sz[0]; size_img[1] = in_sz[1]; size_img[2] = in_sz[2];

    ImagePixelType pixel_type = inimg->getDatatype();
    V3DLONG n_pixels = in_sz[0]*in_sz[1]*in_sz[2];

    unsigned int c = 1;
    unsigned int unit_bites = inimg->getUnitBytes();


    ////convert image to itk format
    ITKImageType::Pointer I  =  ITKImageType::New();
    I =v3d2ItkImage<ITKImageType,ImageScalarTypeUint8>(inimg,in_sz,c);

    ////load filters

    const char *weight_file_im ="/cvlabdata1/cvlab/datasets_amos/data3D/filter_banks/oof/oof_fb_3d_scale_1_2_3_5_8_size_21_weigths_cpd_rank_49.txt";
       const char *sep_filters_file_im ="/cvlabdata1/cvlab/datasets_amos/data3D/filter_banks/oof/oof_fb_3d_scale_1_2_3_5_8_size_21_sep_cpd_rank_49.txt";

  const char *weight_file_ac = "/cvlabdata1/cvlab/datasets_amos/data3D/filter_banks/oof/proto_filter_AC_lap_633_822_weigths_cpd_rank_49.txt";
  const char *sep_filters_file_ac = "/cvlabdata1/cvlab/datasets_amos/data3D/filter_banks/oof/proto_filter_AC_lap_633_822_sep_cpd_rank_49.txt";

    MatrixTypeDouble weights = readMatrix(weight_file_im);
    MatrixTypeDouble sep_filters = readMatrix(sep_filters_file_im);


    MatrixTypeDouble weights_ac = readMatrix(weight_file_ac); //weights_ac = weights_ac/255.0;
    MatrixTypeDouble sep_filters_ac = readMatrix(sep_filters_file_ac);
    MatrixTypeFloat sep_filters_float_ac = sep_filters_ac.cast<float>();
    MatrixTypeFloat weights_float_ac = weights_ac.cast<float>();



    unsigned int n_img_features = weights.cols();
    unsigned int n_ac_features = weights_ac.cols();
    ////convolve image

    const float scale_factor =1.0;
    MatrixTypeFloat sep_filters_float = sep_filters.cast<float>();
    MatrixTypeFloat weights_float = weights.cast<float>();

    TreeBoosterType::ResponseArrayType finalScores = TreeBoosterType::ResponseArrayType::Zero(n_pixels);

    ITKImageType::Pointer pred_img_itk;
    ITKImageType::Pointer predImg_scaled_original_size ;
     Image4DSimple outimg1;


     ITKImageType::Pointer predImg;

    TreeBoosterType::ResponseArrayType newScores_original_size;
    TreeBoosterType::ResponseArrayType newScores;

    VectorTypeFloat scales;
    ITKImageType::Pointer I_resized;
    if(multiscale_pred){
         scales = VectorTypeFloat::Zero(3);
        scales << 0.5 ,1, 2 ;
    }else{
         scales = VectorTypeFloat::Ones(1);
    }
    unsigned int n_scales = scales.size();
    for(unsigned int i_scale=0; i_scale<n_scales; i_scale++){

        std::cout << "Predicting scale:" << i_scale+1 <<"/"<< n_scales<<std::endl;

        V3DLONG *out_sz_res= new V3DLONG[4];

        if(scales(i_scale) ==1){
            out_sz_res[0] = in_sz[0];  out_sz_res[1] = in_sz[1];  out_sz_res[2] = in_sz[2]; out_sz_res[3] = 1;
            I_resized = I;
        }else{


            double *dfactor = new double[3];
            dfactor[0] = scales(i_scale); dfactor[1] = scales(i_scale);  dfactor[2] =scales(i_scale);
            I_resized = resize_image_v3d<ITKImageType,unsigned char>(inimg,dfactor,out_sz_res);

        }

        ITKImageType::SizeType  size_img_scaled;
        size_img_scaled = I_resized->GetLargestPossibleRegion().GetSize();
        V3DLONG n_pixels_scaleed = size_img_scaled[0]*size_img_scaled[1]*size_img_scaled[2];

    MatrixTypeFloat nonsep_features_all;
   std::cout << "Computing features..."<<std::endl;
    computeFeaturesSepComb<ITKImageType,MatrixTypeFloat,VectorTypeFloat>(nonsep_features_all,I_resized,sep_filters_float,weights_float, scale_factor);
std::cout << "Computing features...Done."<<std::endl;



      //MatrixTypeFloat features_ac;


    //  ITKImageType::PixelType max_pred;
    //  ITKImageType::PixelType min_pred;

      for(unsigned int i_ac=0; i_ac<=n_ac_iters; i_ac++){

           cout<<"Auto-Context Iter "<< i_ac<< "/" <<n_ac_iters<< endl;
          regressor_filename = (paras.at(i_ac));

          //MatrixTypeFloat features_image_and_ac;
//          if(i_ac>0){
//              features_image_and_ac =  MatrixTypeFloat::Zero(nonsep_features_all.rows(),nonsep_features_all.cols()+features_ac.cols()) ;
//              features_image_and_ac.middleCols(0,nonsep_features_all.cols()) = nonsep_features_all;
//              features_image_and_ac.middleCols(nonsep_features_all.cols(),features_ac.cols()) = features_ac;

//          }else{
//              features_image_and_ac = nonsep_features_all;

//          }

            //TreeBoosterType::ResponseArrayType newScores = predictRegressor(regressor_filename,features_image_and_ac);
          cout<<"applying regressor:  "<<  regressor_filename<< endl;
           predictRegressor(regressor_filename,nonsep_features_all,newScores);
            predImg = eigenVector2itkImage<ITKImageType,VectorTypeFloat>(newScores.cast<float>(),size_img_scaled);

            if(i_ac<n_ac_iters && n_ac_iters>0){


//                typedef itk::MinimumMaximumImageCalculator <ITKImageType>
//                          ImageCalculatorFilterType;
//                ImageCalculatorFilterType::Pointer imageCalculatorFilter
//                          = ImageCalculatorFilterType::New ();
//                  imageCalculatorFilter->SetImage(predImg);
//                  imageCalculatorFilter->Compute();
// max_pred = imageCalculatorFilter->GetMaximum();
// min_pred = imageCalculatorFilter->GetMinimum();

                //  features_image_and_ac =  MatrixTypeFloat::Zero(nonsep_features_all.rows(),nonsep_features_all.cols()+features_ac.cols()) ;
                if(i_ac<1){
                    nonsep_features_all.conservativeResize(nonsep_features_all.rows(), n_img_features+ n_ac_features);
                }
                //  features_image_and_ac.middleCols(0,nonsep_features_all.cols()) = nonsep_features_all;
                //  features_image_and_ac.middleCols(nonsep_features_all.cols(),features_ac.cols()) = features_ac;
               //    predictRegressor(regressor_filename,nonsep_features_all,newScores);


                //compute features auto-context
                std::cout << "Computing features Auto-context..."<<std::endl;
//                computeFeaturesSepComb<ITKImageType,MatrixTypeFloat,VectorTypeFloat>(features_ac,predImg,sep_filters_float_ac,weights_float_ac, scale_factor);
                MatrixTypeFloat temp_features_ac;
                computeFeaturesSepComb<ITKImageType,MatrixTypeFloat,VectorTypeFloat>(temp_features_ac,predImg,sep_filters_float_ac,weights_float_ac, scale_factor);
                nonsep_features_all.middleCols(n_img_features,n_ac_features) = temp_features_ac;
                temp_features_ac.resize(0,0);
                std::cout << "Computing features Auto-context...Done."<<std::endl;

            }//end compute features auto-context


            if(i_ac==n_ac_iters){

                if(scales(i_scale) ==1){
                    newScores_original_size = newScores;
                }else{

                    std::cout << "Resampling output to orignal size" <<std::endl;

                    V3DLONG *out_sz_back = new V3DLONG[4];
                double *scale_factor_inv = new double[3];
                 scale_factor_inv[0] = ((double)in_sz[0])/((double)out_sz_res[0]);
                 scale_factor_inv[1] = ((double)in_sz[1])/((double)out_sz_res[1]);
                 scale_factor_inv[2] = ((double)in_sz[2])/((double)out_sz_res[2]);

           //     std::cout << "scale_factor_inv: " << scale_factor_inv[0]<< " "<<  scale_factor_inv[1] << " " <<scale_factor_inv[2]<< endl;

                    ITKImageType::SizeType size_res; size_res[0] = out_sz_res[0]; size_res[1] = out_sz_res[1]; size_res[2] = out_sz_res[2];
                   pred_img_itk =  eigenVector2itkImage<ITKImageType,TreeBoosterType::ResponseArrayType>(newScores, size_res);
                   Image4DSimple pred_img_v3d =  itk2v3dImage<ITKImageType>(pred_img_itk);
                 //   std::cout << "RESAMPLING PRED: " << endl;

                    predImg_scaled_original_size = resize_image_v3d<ITKImageType,float>(&pred_img_v3d,scale_factor_inv,out_sz_back);
//                 //   std::cout << "RESAMPLING SIZE: " << out_sz_back[0]<< "IN SIZE: "<<in_sz[0]  <<endl;

                   newScores_original_size = itkImage2EigenVector<ITKImageType,TreeBoosterType::ResponseArrayType>(predImg_scaled_original_size,n_pixels,n_pixels);
                    std::cout << "Resampling output Done." <<std::endl;
                }
          //       std::cout << "summin output scale" <<i_scale <<std::endl;
          //       std::cout << "score orig 0: " <<finalScores[0] <<std::endl;
          //        std::cout << "score rescaled 0: " <<newScores_original_size[0] <<std::endl;

               finalScores +=newScores_original_size;


            }

                ////save image
                cout<<"saving image"<<endl;
                // save image

                //copy data to save with v3d format
               // V3DLONG n_pixels = in_sz[0]*in_sz[1]*in_sz[2];
               float* out_data_copy = new float[n_pixels_scaleed];
             for(unsigned int i_pix = 0; i_pix < n_pixels_scaleed; i_pix++){
                    out_data_copy[i_pix] = (float)newScores(i_pix);
               }


         outimg1.setData((unsigned char *)(out_data_copy), size_img_scaled[0], size_img_scaled[1], size_img_scaled[2], 1, V3D_FLOAT32);



                char outimg_file_ac [500];

                    sprintf (outimg_file_ac, "/cvlabdata1/home/asironi/vaa3d/vaa3d_tools/bigneuron_ported/AmosSironi_PrzemyslawGlowacki/SQBTree_plugin/regression/temp_results/score_img_%i_ac_%i_scale_%i.v3draw",imgInd,i_ac,i_scale);

                    cout << "saving: " << outimg_file_ac<<endl;
                  callback.saveImage(&outimg1, outimg_file_ac);


  }// end auto-context for loop


  }//end scale for loop

    finalScores /= n_scales;


  //  std::cout << "max score: "<< finalScores.maxCoeff() << "min score: "<< finalScores.minCoeff()    << endl;

    cout << "saving final result to: " << outimg_file<<endl;
    Image4DSimple outimg_final;

    float* out_data_copy_final = new float[n_pixels];
    for(unsigned int i_pix = 0; i_pix < n_pixels; i_pix++){
             out_data_copy_final[i_pix] = (float)finalScores(i_pix);
    }

     outimg_final.setData((unsigned char *)(out_data_copy_final), in_sz[0], in_sz[1], in_sz[2], 1, V3D_FLOAT32);
     callback.saveImage(&outimg_final, outimg_file);

 cout << "Saved final result to: " << outimg_file<<endl;

  }// end image for loop

   cout<<"Testing Terminated Normally"<<endl;
  return true;


}// end test funciton

