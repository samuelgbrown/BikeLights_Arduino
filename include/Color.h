#ifndef Color_h
#define Color_h

#include "Arduino.h"
#include "Definitions.h"
#include "Speedometer.h"
#include <math.h>

struct colorObj
{
public:
  colorObj();                                                                   // Constructor
  colorObj(unsigned char r, unsigned char g, unsigned char b, unsigned char w); // Constructor
  colorObj(unsigned char *cA);                                                  // Constructor

  colorObj &operator=(const colorObj &cO); // Assignment operator

  // Get colors
  unsigned char r() const;
  unsigned char g() const;
  unsigned char b() const;
  unsigned char w() const;

  // Adjust brightnesses
  void multiplyBrightness(float brightnessFac); // Multiply the brightnesses of this color by a given value
  //    void addBrightness(int brightnessSubVal); // Add a value to the brightness of this color

  // Color values
  unsigned char c[4]; // Array that contains the value for each color
                      //    unsigned char R = 0;
                      //    unsigned char G = 0;
                      //    unsigned char B = 0;
                      //    unsigned char W = 0;
};

// Color base class that serves as a wrapper for the colorObj (either stores a colorObj or creates one on the fly)
class Color_
{
public:
  Color_(){};
  virtual ~Color_() = default;

  virtual Color_ *clone() const = 0;
  // Getting color methods
  virtual colorObj getColor() const = 0;
  //    virtual unsigned char r() = 0;
  //    virtual unsigned char g() = 0;
  //    virtual unsigned char b() = 0;
  //    virtual unsigned char w() = 0;

  virtual COLOR_TYPE getType() const = 0;

  Color_ *getColor_(); // Get a Color_* pointer from any of the derived classes

  // boolean isThisEmpty() const; // Public interface to see if this object is empty

protected:
  // boolean isEmpty = false; // Does this Color_ represent a pure black empty color object? (Can only be true by getting an unmodified Color_Static object instantiated by the blank constructor, Color_Static())
};

class Color_Static : public Color_
{
public:
  Color_Static();                                                                   // Constructor
  Color_Static(colorObj c);                                                         // Constructor
  Color_Static(unsigned char r, unsigned char g, unsigned char b, unsigned char w); // Constructor
  Color_Static(unsigned char * cA);                                                 // Constructor

  Color_Static(const Color_Static &c); // Copy Constructor

  COLOR_TYPE getType() const
  {
    return COLOR_STATIC;
  };

  Color_Static *clone() const;

  // Get color
  colorObj getColor() const;
  //    unsigned char r();
  //    unsigned char g();
  //    unsigned char b();
  //    unsigned char w();

  void setColor(colorObj c);

  // Set each color
  void rS(unsigned char rNew);
  void gS(unsigned char gNew);
  void bS(unsigned char bNew);
  void wS(unsigned char wNew);
  void cS(colorObj cNew);

private:
  colorObj c;
};

template <class T>
class Color_d : public Color_
{
public:
  Color_d();                                                             // Constructor
  Color_d(unsigned char numColors);                                      // Constructor
  Color_d(colorObj *cA, T *tA, BLEND_TYPE *bA, unsigned char numColors); // Constructor

  Color_d(const Color_d &c); // Copy Constructor

  ~Color_d(); // Destructor

  // Get each color
  colorObj getColor() const;
  //    unsigned char r();
  //    unsigned char g();
  //    unsigned char b();
  //    unsigned char w();

  // Get values in each array (to view or modify)
  unsigned char getNumColors() const {return numColors;};                           // Get the total number of colorObjs in this Color_
  colorObj &getThisColorObj(unsigned char numInArray) const;    // Sorted according to increasing values of Trigger
  T getThisTrigger(unsigned char numInArray) const;             // Sorted according to increasing values of Trigger
  BLEND_TYPE &getThisBlendType(unsigned char numInArray) const; // Sorted according to increasing values of Trigger

  colorObj *&getAllColorObjs() const;    // Return an array with all of the color objects
  BLEND_TYPE *&getAllBlendTypes() const; // Return an array with all of the blend types
  T *getAllTriggers() const;             // Return an array with all of the triggers

  // Set values in tA
  void setThisTrigger(T tNew, unsigned char numInArray);
  void setThisColorObj(colorObj colorObjNew, unsigned char numInArray);
  void setThisBlendType(BLEND_TYPE blendTypeNew, unsigned char numInArray);

  // Set up the arrays in this object
  void setupArrays(unsigned char numColors);
  void setupArrays(colorObj *cA, T *tA, BLEND_TYPE *bA, unsigned char numColorsIn);

  //    void rS(unsigned char rNew);
  //    void gS(unsigned char gNew);
  //    void bS(unsigned char bNew);
  //    void wS(unsigned char wNew);

protected:
  virtual T getCurVal() const = 0; // Use a method to get the current value of whatever the trigger type is
  //    unsigned int lastBlendLoc = 0; // Save the last blending location, to cut down on a few cycles

  // Stored arrays and metadata
  colorObj *cA = NULL;
  T *tA = NULL; // Always sorted after being added via setupArrays
  BLEND_TYPE *bA = NULL;
  unsigned char numColors = 0; // Size of the three parallel arrays
private:
  void deleteAllArrays();
  void sortAllArrays(); // Sort all of the arrays according to increasing Trigger

  colorObj blendColors(colorObj c1, colorObj c2, float ratio) const;               // Get a blend between c1 and c2 according to the ratio (between 0 and 1)
  unsigned char blendChars(unsigned char c1, unsigned char c2, float ratio) const; // Get a linearly scaled ratio between c1 and c2 according to ratio (between 0 and 1)
};

class Color_dTime : public Color_d<unsigned long>
{
  // Trigger values are a measure of time, in milliseconds
public:
  Color_dTime();                                                                         // Constructor
  Color_dTime(unsigned char numColors);                                                  // Constructor
  Color_dTime(colorObj *cA, unsigned long *tA, BLEND_TYPE *bA, unsigned char numColors); // Constructor

  Color_dTime(const Color_dTime &c); // Copy Constructor

  ~Color_dTime(); // Destructor

  Color_dTime *clone() const;

  COLOR_TYPE getType() const
  {
    return COLOR_DTIME;
  };

  unsigned long getCurVal() const; // Use a method to get the current value of the time

  // Modified Color_d functions
  // Set values in tA
  void setThisTrigger(unsigned long tNew, unsigned char numInArray);

  // Set up the arrays in this object
  //    void setupArrays(unsigned char numColors);
  //    void setupArrays(colorObj* cA, unsigned long* tA, BLEND_TYPE* bA, unsigned char numColorsIn);

private:
  //    unsigned long maxTriggerInArray = 0;
  //    void getMaxTrigger();
};

class Color_dVel : public Color_d<float>
{
public:
  Color_dVel(Speedometer *speedometer);                                                                   // Constructor
  Color_dVel(Speedometer *speedometer, colorObj *cA, float *tA, BLEND_TYPE *bA, unsigned char numColors); // Constructor
  Color_dVel *clone() const;

  COLOR_TYPE getType() const
  {
    return COLOR_DVEL;
  };

  float getCurVal() const; // Use a method to get the current value of the velocity

private:
  Speedometer *speedometer = NULL;
};
#endif
