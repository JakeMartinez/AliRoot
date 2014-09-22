/* $Id$ */

/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/**
 * >> Flat structure representing a TPC seed <<
 *
 * To be used in the online and offline calibration schema.
 *
 *
 * Primary Authors : Sergey Gorbunov, Jochen Thaeder, Chiara Zampolli
 *
 **************************************************************************/

#include "AliFlatTPCseed.h"
#include "AliTPCseed.h"
#include "AliHLTTPCTransform.h"
#include "Riostream.h"


void AliFlatTPCseed::SetFromTPCseed( const AliTPCseed *p )
{
  // initialise from AliTPCseed

  Reset();
  if( !p ) return;

  fParam.SetExternalTrackParam(  p );
  fLabel = p->GetLabel();
  AliFlatTPCCluster *clusters = reinterpret_cast< AliFlatTPCCluster* >( fContent );  
  for( Int_t irow=0; irow<160; irow++ ){
    const AliTPCclusterMI *cl = p->GetClusterPointer(irow);
    if( !cl ) continue;
    AliFlatTPCCluster &flatCluster = clusters[fNTPCClusters];
    flatCluster.SetTPCCluster( cl );
    fNTPCClusters++;
  }
}

void AliFlatTPCseed::GetTPCseed( AliTPCseed *p ) const
{
   // write to AliTPCseed
  if( !p ) return;
  p->Reset();

  AliTPCseed seed;

  fParam.GetExternalTrackParam( seed );
  seed.SetLabel(fLabel);  
  seed.SetNumberOfClusters(fNTPCClusters);

  AliTPCclusterMI clusters[fNTPCClusters];

  const AliFlatTPCCluster *flatClusters = reinterpret_cast< const AliFlatTPCCluster* >( fContent );

  for( Int_t ic=0; ic<fNTPCClusters; ic++){
    const AliFlatTPCCluster &flatCluster = flatClusters[ic];    
    flatCluster.GetTPCCluster( &(clusters[ic]) );
    int sec = flatCluster.GetSector();
    int row = flatCluster.GetPadRow();
    if(sec >= 36) row = row + AliHLTTPCTransform::GetNRowLow();
    if( row<160 ) seed.SetClusterPointer( row , &(clusters[ic]) );
  }
  new (p) AliTPCseed( seed, kTRUE ); // true means that p creates its own cluster objects
}
