//------------------------------------------------------------------------------------------------
// Copyright (c) 2012  Alessandro Bria and Giulio Iannello (University Campus Bio-Medico of Rome).  
// All rights reserved.
//------------------------------------------------------------------------------------------------

/*******************************************************************************************************************************************************************************************
*    LICENSE NOTICE
********************************************************************************************************************************************************************************************
*    By downloading/using/running/editing/changing any portion of codes in this package you agree to this license. If you do not agree to this license, do not download/use/run/edit/change
*    this code.
********************************************************************************************************************************************************************************************
*    1. This material is free for non-profit research, but needs a special license for any commercial purpose. Please contact Alessandro Bria at a.bria@unicas.it or Giulio Iannello at 
*       g.iannello@unicampus.it for further details.
*    2. You agree to appropriately cite this work in your related studies and publications.
*
*       Bria, A., et al., (2012) "Stitching Terabyte-sized 3D Images Acquired in Confocal Ultramicroscopy", Proceedings of the 9th IEEE International Symposium on Biomedical Imaging.
*       Bria, A., Iannello, G., "A Tool for Fast 3D Automatic Stitching of Teravoxel-sized Datasets", submitted on July 2012 to IEEE Transactions on Information Technology in Biomedicine.
*
*    3. This material is provided by  the copyright holders (Alessandro Bria  and  Giulio Iannello),  University Campus Bio-Medico and contributors "as is" and any express or implied war-
*       ranties, including, but  not limited to,  any implied warranties  of merchantability,  non-infringement, or fitness for a particular purpose are  disclaimed. In no event shall the
*       copyright owners, University Campus Bio-Medico, or contributors be liable for any direct, indirect, incidental, special, exemplary, or  consequential  damages  (including, but not 
*       limited to, procurement of substitute goods or services; loss of use, data, or profits;reasonable royalties; or business interruption) however caused  and on any theory of liabil-
*       ity, whether in contract, strict liability, or tort  (including negligence or otherwise) arising in any way out of the use of this software,  even if advised of the possibility of
*       such damage.
*    4. Neither the name of University  Campus Bio-Medico of Rome, nor Alessandro Bria and Giulio Iannello, may be used to endorse or  promote products  derived from this software without
*       specific prior written permission.
********************************************************************************************************************************************************************************************/

#include "../presentation/PDialogImport.h"
#include "../presentation/PMain.h"
#include "../presentation/PLog.h"
#include "CImport.h"
#include "CPlugin.h"
#include <sstream>
#include <limits>
#include <algorithm>
#include "../core/ImageManager/StackedVolume.h"
#include "../core/ImageManager/TiledVolume.h"

using namespace teramanager;

CImport* CImport::uniqueInstance = NULL;
bool sortVolumesAscendingSize (VirtualVolume* i,VirtualVolume* j) { return (i->getMVoxels() < j->getMVoxels()); }

void CImport::uninstance()
{
    #ifdef TMP_DEBUG
    printf("--------------------- teramanager plugin [thread ?] >> CImport::uninstance()\n");
    #endif

    if(uniqueInstance)
    {
        delete uniqueInstance;
        uniqueInstance = NULL;
    }
}

CImport::~CImport()
{
    #ifdef TMP_DEBUG
    printf("--------------------- teramanager plugin [thread *] >> CImport::~CImport()\n");
    #endif

    for(int k=0; k<volumes.size(); k++)
        if(volumes[k])
            delete volumes[k];

    #ifdef TMP_DEBUG
    printf("--------------------- teramanager plugin [thread *] >> CImport destroyed\n");
    #endif
}

//SET methods
void CImport::setAxes(string axs1, string axs2, string axs3)
{
    #ifdef TMP_DEBUG
    printf("--------------------- teramanager plugin [thread *] >> CImport::setAxes(%s, %s, %s)\n",  axs1.c_str(), axs2.c_str(), axs3.c_str());
    #endif

    AXS_1 = axis(atoi(axs1.c_str()));
    AXS_2 = axis(atoi(axs2.c_str()));
    AXS_3 = axis(atoi(axs3.c_str()));
}
void CImport::setVoxels(std::string vxl1, std::string vxl2, std::string vxl3)
{
    #ifdef TMP_DEBUG
    printf("--------------------- teramanager plugin [thread *] >> CImport::setVoxels(%s, %s, %s)\n",  vxl1.c_str(), vxl2.c_str(), vxl3.c_str());
    #endif

    std::istringstream tmp1(vxl1), tmp2(vxl2), tmp3(vxl3);
    tmp1.imbue(std::locale("C"));
    tmp2.imbue(std::locale("C"));
    tmp3.imbue(std::locale("C"));
    tmp1 >> VXL_1;
    tmp2 >> VXL_2;
    tmp3 >> VXL_3;
}

//automatically called when current thread is started
void CImport::run()
{
    #ifdef TMP_DEBUG
    printf("--------------------- teramanager plugin [thread *] >> CImport::run()\n");
    #endif

    char errMsg[IM_STATIC_STRINGS_SIZE];

    try
    {
        QElapsedTimer timerIO;
        timerIO.start();

        /********************* 1) IMPORTING CURRENT VOLUME ***********************
        If metadata binary file doesn't exist or the volume has to be re-imported,
        further informations must be provided to the constructor.
        *************************************************************************/
        string mdata_fpath = path;
        mdata_fpath.append("/");
        mdata_fpath.append(IM_METADATA_FILE_NAME);
        if(!QFile::exists(mdata_fpath.c_str()) || reimport)
        {
            //checking current members validity
            if(AXS_1 != axis_invalid && AXS_2 != axis_invalid && AXS_3 != axis_invalid && VXL_1 != 0 && VXL_2 != 0 && VXL_3 != 0)
            {
                VirtualVolume* volume = 0;
                try
                {
                    volume = new StackedVolume(path.c_str(), ref_sys(AXS_1,AXS_2,AXS_3),VXL_1,VXL_2,VXL_3, reimport);
                }
                catch(...)
                {
                    try
                    {
                        volume = new TiledVolume(path.c_str(), ref_sys(AXS_1,AXS_2,AXS_3),VXL_1,VXL_2,VXL_3, reimport);
                    }
                    catch(...)
                    {
                        sprintf(errMsg, "Unable to import volume at \"%s\"", path.c_str());
                        throw MyException(errMsg);
                    }
                }
                volumes.push_back(volume);
            }
            else
            {
                sprintf(errMsg, "Invalid parameters AXS_1(%s), AXS_2(%s), AXS_3(%s), VXL_1(%.4f), VXL_2(%.4f), VXL_3(%.4f)",
                        axis_to_str(AXS_1), axis_to_str(AXS_2), axis_to_str(AXS_3), VXL_1, VXL_2, VXL_3);
                throw MyException(errMsg);
            }
        }
        else
        {
            VirtualVolume* volume = 0;
            try
            {
                volume = new StackedVolume(path.c_str(), ref_sys(axis_invalid,axis_invalid,axis_invalid),0,0,0);
            }
            catch(...)
            {
                try
                {
                    volume = new TiledVolume(path.c_str(), ref_sys(axis_invalid,axis_invalid,axis_invalid),0,0,0);
                }
                catch(...)
                {
                    sprintf(errMsg, "Unable to import volume at \"%s\"", path.c_str());
                    throw MyException(errMsg);
                }
            }
            volumes.push_back(volume);
        }

        /********************* 2) IMPORTING OTHER VOLUMES ***********************
        If multiresolution mode is enabled, importing all the available resolutions
        within the current volume's parent directory.
        *************************************************************************/
        if(multiresMode)
        {
            //detecting candidate volumes
            vector<VirtualVolume*> candidateVols;
            QDir curParentDir(path.c_str());
            curParentDir.cdUp();
            QStringList otherDirs = curParentDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
            for(int k=0; k<otherDirs.size(); k++)
            {
                string path_i = curParentDir.absolutePath().append("/").append(otherDirs.at(k).toLocal8Bit().constData()).toStdString();
                try
                {
                    VirtualVolume* candidate_vol = 0;
                    if(dynamic_cast<StackedVolume*>(volumes[0]))
                    {
                        candidate_vol = new StackedVolume(path_i.c_str(),
                                                                         ref_sys(dynamic_cast<StackedVolume*>(volumes[0])->getAXS_1(),
                                                                         dynamic_cast<StackedVolume*>(volumes[0])->getAXS_2(),
                                                                         dynamic_cast<StackedVolume*>(volumes[0])->getAXS_3()),
                                                                         dynamic_cast<StackedVolume*>(volumes[0])->getVXL_1(),
                                                                         dynamic_cast<StackedVolume*>(volumes[0])->getVXL_2(),
                                                                         dynamic_cast<StackedVolume*>(volumes[0])->getVXL_3(), reimport, false);
                    }
                    else if(dynamic_cast<TiledVolume*>(volumes[0]))
                    {
                        candidate_vol = new TiledVolume(path_i.c_str(),
                                                                         ref_sys(dynamic_cast<TiledVolume*>(volumes[0])->getAXS_1(),
                                                                         dynamic_cast<TiledVolume*>(volumes[0])->getAXS_2(),
                                                                         dynamic_cast<TiledVolume*>(volumes[0])->getAXS_3()),
                                                                         dynamic_cast<TiledVolume*>(volumes[0])->getVXL_1(),
                                                                         dynamic_cast<TiledVolume*>(volumes[0])->getVXL_2(),
                                                                         dynamic_cast<TiledVolume*>(volumes[0])->getVXL_3(), reimport, false);
                    }
                    else
                    {
                        sprintf(errMsg, "Unable to import volume at \"%s\"", path_i.c_str());
                        throw MyException(errMsg);
                    }
                    candidateVols.push_back(candidate_vol);
                }
                catch(...){}
            }

            //importing candidate volumes
            for(int k=0; k<candidateVols.size(); k++)
            {
                //current volume (now stored in volumes[0]) should be discarded
                if(candidateVols[k]->getMVoxels() != volumes[0]->getMVoxels())
                {
                    int ratio = pow((volumes[0]->getMVoxels() / candidateVols[k]->getMVoxels()),(1/3.0f)) + 0.5;

                    VirtualVolume* vol = 0;
                    if(dynamic_cast<StackedVolume*>(volumes[0]))
                    {
                        vol = new StackedVolume(candidateVols[k]->getROOT_DIR(),
                                                                         ref_sys(dynamic_cast<StackedVolume*>(volumes[0])->getAXS_1(),
                                                                         dynamic_cast<StackedVolume*>(volumes[0])->getAXS_2(),
                                                                         dynamic_cast<StackedVolume*>(volumes[0])->getAXS_3()),
                                                                         dynamic_cast<StackedVolume*>(volumes[0])->getVXL_1()*ratio,
                                                                         dynamic_cast<StackedVolume*>(volumes[0])->getVXL_2()*ratio,
                                                                         dynamic_cast<StackedVolume*>(volumes[0])->getVXL_3()*ratio, reimport);
                    }
                    else if(dynamic_cast<TiledVolume*>(volumes[0]))
                    {
                        vol = new TiledVolume(candidateVols[k]->getROOT_DIR(),
                                                                         ref_sys(dynamic_cast<TiledVolume*>(volumes[0])->getAXS_1(),
                                                                         dynamic_cast<TiledVolume*>(volumes[0])->getAXS_2(),
                                                                         dynamic_cast<TiledVolume*>(volumes[0])->getAXS_3()),
                                                                         dynamic_cast<TiledVolume*>(volumes[0])->getVXL_1()*ratio,
                                                                         dynamic_cast<TiledVolume*>(volumes[0])->getVXL_2()*ratio,
                                                                         dynamic_cast<TiledVolume*>(volumes[0])->getVXL_3()*ratio, reimport);
                    }
                    else
                    {
                        sprintf(errMsg, "Unable to import volume at \"%s\"", candidateVols[k]->getROOT_DIR());
                        throw MyException(errMsg);
                    }
                    volumes.push_back(vol);

                    delete candidateVols[k];
                }
            }

            //check the number of volumes (at least 2)
            if(volumes.size() < 2)
                throw MyException("One resolution found only: at least two resolutions are needed for the multiresolution mode.");

            //sorting volumes by ascending size
            std::sort(volumes.begin(), volumes.end(), sortVolumesAscendingSize);
        }

        /********************** 3) GENERATING VOLUME 3D MAP ***********************
        If multiresolution mode is enabled, it could be convenient to generate once
        for all a volume map from a low-resolution volume.
        *************************************************************************/
        Image4DSimple* volMapImage = 0;
        if(multiresMode)
        {
            //searching for an already existing map, if not available we try to generate it from the lower resolutions
            string volMapPath = path;
            volMapPath.append("/");
            volMapPath.append(TMP_VMAP_FNAME);
            if(!StackedVolume::fileExists(volMapPath.c_str()) || reimport || regenerateVolMap)
            {
                //searching for the highest resolution available which size is less then the maximum allowed
                int volMapIndex = -1;
                for(int k=0; k<volumes.size(); k++)
                {
                    if(volumes[k]->getMVoxels() < volMapMaxSize)
                        volMapIndex = k;
                }

                //if found, generating and saving the corresponding map, otherwise throwing an exception
                if(volMapIndex != -1)
                {
                    uint8* vmap_raw = volumes[volMapIndex]->loadSubvolume_to_UINT8(-1, -1, -1, -1, -1, -1, &nchannels);
                    volMapHeight = volumes[volMapIndex]->getDIM_V();
                    volMapWidth  = volumes[volMapIndex]->getDIM_H();
                    volMapDepth  = volumes[volMapIndex]->getDIM_D();
                    FILE *volMapBin = fopen(volMapPath.c_str(), "wb");

                    // --- Alessandro 2013-04-23: added exception when file can't be opened in write mode
                    if(!volMapBin)
                        throw MyException(QString("Cannot write volume map at \"").append(volMapPath.c_str()).append("\"\n\nPlease check write permissions on this storage.").toStdString().c_str());

                    uint16 verstr_size = static_cast<uint16>(strlen(CPlugin::getMajorVersion().c_str()) + 1);
                    fwrite(&verstr_size, sizeof(uint16), 1, volMapBin);
                    fwrite(CPlugin::getMajorVersion().c_str(), verstr_size, 1, volMapBin);
                    fwrite(&nchannels, sizeof(int), 1, volMapBin);
                    fwrite(&volMapHeight, sizeof(uint32), 1, volMapBin);
                    fwrite(&volMapWidth,  sizeof(uint32), 1, volMapBin);
                    fwrite(&volMapDepth,  sizeof(uint32), 1, volMapBin);
                    fwrite(vmap_raw, volMapHeight*volMapWidth*volMapDepth*nchannels, 1, volMapBin);
                    fclose(volMapBin);
                }
                else
                {
                    QString msg = "Can't generate 3D volume map: the volume map size limit is "+
                                  QString::number(volMapMaxSize) + " MVoxels while the lower resolution found has size " +
                                  QString::number(volumes[0]->getMVoxels(), 'f', 0) + " MVoxels";

                    throw MyException(msg.toStdString().c_str());
                }
            }

            //at this point we should have the volume map stored in the volume's directory
            size_t fread_return_val;
            FILE *volMapBin = fopen(volMapPath.c_str(), "rb");

            // --- Alessandro 2013-04-23: added exception when file can't be opened in read mode
            if(!volMapBin)
                throw MyException(QString("Cannot read volume map at \"").append(volMapPath.c_str()).append("\"\n\nPlease check read permissions on this storage or whether the path is correct.").toStdString().c_str());


            //checking plugin version
            uint16 verstr_size;
            fread_return_val = fread(&verstr_size, sizeof(uint16), 1, volMapBin);
            if(fread_return_val != 1)
                throw MyException("Unable to read volume map file (<version_size> field). It must be regenerated.");
            char *version = new char[verstr_size];
            fread_return_val = fread(version, verstr_size, 1, volMapBin);
            if(fread_return_val != 1)
                throw MyException("Unable to read volume map file (<version> field). It must be regenerated.");
            if(atof(version) < 0.7f)
                throw MyException(QString("Volume map file was generated with a plugin version"
                                  " (").append(version).append(") older than 0.7. Please check the \"Regenerate volume map\" option from \"File->Options\".").toStdString().c_str());
            delete[] version;

            //loading metadata and data
            fread_return_val = fread(&nchannels, sizeof(int), 1, volMapBin);
            if(fread_return_val != 1)
                throw MyException("Unable to read volume map file (<nchannels> field). It must be regenerated.");
            fread_return_val = fread(&volMapHeight, sizeof(uint32), 1, volMapBin);
            if(fread_return_val != 1)
                throw MyException("Unable to read volume map file (<volMapHeight> field). It must be regenerated.");
            fread_return_val = fread(&volMapWidth,  sizeof(uint32), 1, volMapBin);
            if(fread_return_val != 1)
                throw MyException("Unable to read volume map file (<volMapWidth> field). It must be regenerated.");
            fread_return_val = fread(&volMapDepth,  sizeof(uint32), 1, volMapBin);
            if(fread_return_val != 1)
                throw MyException("Unable to read volume map file (<volMapDepth> field). It must be regenerated.");
            volMapData = new uint8[volMapHeight*volMapWidth*volMapDepth*nchannels];
            fread_return_val = fread(volMapData, volMapHeight*volMapWidth*volMapDepth*nchannels, 1, volMapBin);
            if(fread_return_val != 1)
                throw MyException("Unable to read volume map file (<volMapData> field). It must be regenerated.");
            fclose(volMapBin);
            volMapImage = new Image4DSimple();
            volMapImage->setFileName("VolumeMap");
            volMapImage->setData(volMapData, volMapWidth, volMapHeight, volMapDepth, nchannels, V3D_UINT8);
        }

        //everything went OK
        emit sendOperationOutcome(0, volMapImage, timerIO.elapsed());
    }
    catch( MyException& exception)  {reset(); emit sendOperationOutcome(new MyException(exception.what()), 0);}
    catch(const char* error)        {reset(); emit sendOperationOutcome(new MyException(error), 0);}
    catch(...)                      {reset(); emit sendOperationOutcome(new MyException("Unknown error occurred"), 0);}
}
