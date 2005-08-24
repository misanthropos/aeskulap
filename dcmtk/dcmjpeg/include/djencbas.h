/*
 *
 *  Copyright (C) 1997-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmjpeg
 *
 *  Author:  Marco Eichelberg, Norbert Olges
 *
 *  Purpose: Codec class for encoding JPEG Baseline (lossy, 8-bit)
 *
 *  Last Update:      $Author: braindead $
 *  Update Date:      $Date: 2005/08/23 19:32:02 $
 *  Source File:      $Source: /cvsroot/aeskulap/aeskulap/dcmtk/dcmjpeg/include/Attic/djencbas.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DJENCBAS_H
#define DJENCBAS_H

#include "osconfig.h"
#include "djcodece.h" /* for class DJCodecEncoder */


/** Encoder class for JPEG Baseline (lossy, 8-bit)
 */
class DJEncoderBaseline : public DJCodecEncoder
{
public: 

  /// default constructor
  DJEncoderBaseline();

  /// destructor
  virtual ~DJEncoderBaseline();

  /** returns the transfer syntax that this particular codec
   *  is able to encode and decode.
   *  @return supported transfer syntax
   */
  virtual E_TransferSyntax supportedTransferSyntax() const;

private:

  /** returns true if the transfer syntax supported by this
   *  codec is lossless.
   *  @return lossless flag
   */
  virtual OFBool isLosslessProcess() const;

  /** creates 'derivation description' string after encoding.
   *  @param toRepParam representation parameter passed to encode()
   *  @param cp codec parameter passed to encode()
   *  @param bitsPerSample bits per sample of the original image data prior to compression
   *  @param ratio image compression ratio. This is not the "quality factor"
   *    but the real effective ratio between compressed and uncompressed image,
   *    i. e. 30 means a 30:1 lossy compression.
   *  @param imageComments image comments returned in this
   *    parameter which is initially empty
   */
  virtual void createDerivationDescription(
    const DcmRepresentationParameter * toRepParam,
    const DJCodecParameter *cp,
    Uint8 bitsPerSample,
    double ratio,
    OFString& derivationDescription) const;

  /** creates an instance of the compression library to be used
   *  for encoding/decoding.
   *  @param toRepParam representation parameter passed to encode()
   *  @param cp codec parameter passed to encode()
   *  @param bitsPerSample bits per sample for the image data
   *  @return pointer to newly allocated codec object
   */
  virtual DJEncoder *createEncoderInstance(
    const DcmRepresentationParameter * toRepParam,
    const DJCodecParameter *cp,
    Uint8 bitsPerSample) const;

};

#endif

/*
 * CVS/RCS Log
 * $Log: djencbas.h,v $
 * Revision 1.1  2005/08/23 19:32:02  braindead
 * - initial savannah import
 *
 * Revision 1.1  2005/06/26 19:26:09  pipelka
 * - added dcmtk
 *
 * Revision 1.1  2001/11/13 15:56:25  meichel
 * Initial release of module dcmjpeg
 *
 *
 */