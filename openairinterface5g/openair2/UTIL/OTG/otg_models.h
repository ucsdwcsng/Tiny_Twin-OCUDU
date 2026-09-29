/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief Data structure and functions for OTG
 */

#ifndef __OTG_MODELS_H__
# define __OTG_MODELS_H__



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>




/*function declarations*/


/*tarma*/
double tarmaCalculateSample( double inputSamples[], tarmaProcess_t *proc);
void tarmaUpdateInputSample (tarmaStream_t *stream);
tarmaStream_t *tarmaInitStream(tarmaStream_t *stream);
void tarmaSetupOpenarenaDownlink(tarmaStream_t *stream);
void tarmaPrintProc(tarmaProcess_t *proc);
void tarmaPrintStreamInit(tarmaStream_t *stream);

/*tarma video*/
void tarmaPrintVideoInit(tarmaVideo_t *video);
tarmaVideo_t *tarmaInitVideo(tarmaVideo_t *video);
double tarmaCalculateVideoSample(tarmaVideo_t *video);
void tarmaSetupVideoGop12(tarmaVideo_t *video, double compression);

/*background*/
backgroundStream_t *backgroundStreamInit(backgroundStream_t *stream, double lambda_n);
void backgroundUpdateStream(backgroundStream_t *stream, int ctime);
double backgroundCalculateSize(backgroundStream_t *stream, int ctime, int idt);
void backgroundPrintStream(backgroundStream_t *stream);

#endif
