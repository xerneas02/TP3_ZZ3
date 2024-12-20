// $Id: JamesRandom.h,v 1.16 2002/04/12 15:02:44 evc Exp $
// -*- C++ -*-
//
// -----------------------------------------------------------------------
//                             HEP Random
//                        --- NiedQRNG ---
//                          class header file
// -----------------------------------------------------------------------
// This file is part of Geant4 (simulation toolkit for HEP).
//
// NiedQRNG implements the algorithm by Marsaglia-Zaman RANMAR
// described in "F.James, Comp. Phys. Comm. 60 (1990) 329" and implemented
// in FORTRAN77 as part of the MATHLIB HEP library for pseudo-random
// numbers generation.
// This is the default random engine invoked by each distribution unless
// the user sets a different one.

// =======================================================================
// Gabriele Cosmo - Created: 5th September 1995
//                - Minor corrections: 31st October 1996
//                - Added methods for engine status: 19th November 1996
//                - setSeed(), setSeeds() now have default dummy argument
//                  set to zero: 11th July 1997
// J.Marraffino   - Added stream operators and related constructor.
//                  Added automatic seed selection from seed table and
//                  engine counter: 16th Feb 1998
// Ken Smith      - Added conversion operators:  6th Aug 1998
// V. Innocente   - changed pointers to indices     3 may 2000
// =======================================================================

#ifndef NiedQRNG_h
#define NiedQRNG_h 1


#include <gsl/gsl_qrng.h>
#include <string>

#include "CLHEP/Random/RandomEngine.h"


class Exception{
  public:
    Exception(){};
    Exception(std::string inMsg){msg = inMsg;}
  protected:
    std::string msg;
};


/**
 * @author
 * @ingroup random
 */
class NiedQRNG: public HepRandomEngine {

public:

  //NiedQRNG(HepStd::istream& is);
  NiedQRNG();
  //NiedQRNG(long seed);
  //NiedQRNG(int rowIndex, int colIndex);
  virtual ~NiedQRNG();
  // Constructor and destructor.

  //NiedQRNG(const NiedQRNG &p);
  // Copy constructor

  //NiedQRNG & operator = (const NiedQRNG &p);
  // Overloaded assignment operator, to retrieve the engine status.

  double flat();
  // Returns a pseudo random number between 0 and 1 
  // (excluding the end points)

  void flatArray (const int size, double* vect);
  // Fills the array "vect" of specified size with flat random values.

 void setSeed(long seed, int dum=0){throw new Exception("Methode non implementee");};
  // Sets the state of the algorithm according to seed.

  void setSeeds(const long * seeds, int dum=0){throw new Exception("Methode non implementee");};
  // Sets the state of the algorithm according to the zero terminated
  // array of seeds. Only the first seed is used.

  void saveStatus( const char filename[] = "JamesRand.conf" ) const{throw new Exception("Methode non implementee");};
  // Saves on file JamesRand.conf the current engine status.

  void restoreStatus( const char filename[] = "JamesRand.conf" ){throw new Exception("Methode non implementee");};
  // Reads from file JamesRand.conf the last saved engine status
  // and restores it.

  void showStatus() const{throw new Exception("Methode non implementee");};
  // Dumps the engine status on the screen.

 // operator unsigned int();
  // 32-bit flat, but slower than double or float.

  //friend HepStd::ostream& operator<< (HepStd::ostream& os, const NiedQRNG& e);
 // friend HepStd::istream& operator>> (HepStd::istream& is,       NiedQRNG& e);

private:

  // Members defining the current status of the generator.
  /*double u[97];
  double c, cd, cm;
  int i97, j97;
  static int numEngines;
  static int maxIndex;*/
  gsl_qrng * q;

};

#endif
