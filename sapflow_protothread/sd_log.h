#pragma once

#include <SPI.h>
#include <SdFat.h>
#include <sdios.h> //for ArduinoOutStream
#include "pinout.h"
#include "ftoa.h" // from DSPlite, for string conversion

/** @file */

/** Allows use of streams to print to Serial via cout 
@returns an ArduinoOutStream. Please don't use the return value. */
static ArduinoOutStream cout(Serial);

static SdFat sd; ///< File system object.

/** Avoids file conflicts by renaming

Checks if the filename exists.
If so, appends an integer.
For example, if "test.csv" and "test(01).csv" are present in the file system and you call
    String x = newfile("test");
x will contain the string "test(02).csv"

@param fname The desired file name (without the extension)
@param suffix The file extension
@returns the new filename it came up with.
*/
String newfile( String fname, String suffix=".csv" );


/** Converts an integer to a string
 * 
 * @param x The integer
 * @returns a string containing a text representation of x
 */
String int2str( int32_t x );