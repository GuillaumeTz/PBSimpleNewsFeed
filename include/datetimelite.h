/*
The MIT License

Copyright (c) 2011 lyo.kato@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _DATETIMELITE_H_
#define _DATETIMELITE_H_
#include <string>
#include <cstring>
#include <stdexcept>

#define HANDLE_EXCEPTION(msg) \
  throw std::invalid_argument(msg)

#define SET_V(n) \
    std::memcpy(buf, c - n, 4); \
    buf[n] = '\0'; \
    v = std::atoi(buf)

#define STEP_RANGE(c, i1, i2) \
  if (!(*c >= i1 && *c <= i2)) \
    HANDLE_EXCEPTION("format not supported: wrong range"); \
  ++c

#define IS_DIGIT(c) \
    ((c) >= '0' && (c) <= '9')

#define IS_ALNUM(c) \
    (((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))

#define IS_ALPHA(c) \
    (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))

#define STEP_DIGIT(c) \
  if (!IS_DIGIT(*c)) \
    HANDLE_EXCEPTION("format not supported: should be digit"); \
  ++c

#define STEP_DELIMITER(c) \
    if (!(*c == ' ' || *c == '-' || *c == '/')) \
      HANDLE_EXCEPTION("format not supported: delimiter not found"); \
    ++c

#define STEP_IF_DELIMITER(c) \
  if (*c == ' ' || *c == '-' || *c == '/') \
    ++c

#define RETURN_TYPE struct std::tm

#define WRAP_RESULT(ts) ts

namespace datetimelite {

static bool
is_leap_year(unsigned short year)
{
  return (year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0));
}

static int
days_in_month(unsigned short year, unsigned short month)
{
  static const unsigned short table[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  if ( month == 0 || month > 12 )
    return 0;
  if ( month == 2 && is_leap_year(year))
    return 29;
  return table[month-1];
}

static bool
check_date(unsigned short year, unsigned short month, unsigned short mday)
{
  return ((month > 0)
    && (month < 13)
    && (mday > 0)
    && (mday <= days_in_month(year, month)));
}

static RETURN_TYPE
time_from_string(const std::string& s)
{
  struct std::tm ts;
  std::memset(&ts, 0, sizeof(ts));
  
  const char *c = s.c_str();
  char buf[5];
  int v;

  // if found weekday, skip it.
  if (IS_ALPHA(*c)) {
    while (IS_ALPHA(*c)) {
      ++c;
    }
    if (*c == ',')
      ++c;
    if (*c != ' ')
      HANDLE_EXCEPTION("format not supported: weekday sepratator");
    ++c;
  }

  STEP_DIGIT(c);
  STEP_DIGIT(c);
  if (*c == '\0')
    HANDLE_EXCEPTION("format not supported: too short");
  if (IS_DIGIT(*c)) {
    ++c;
    STEP_DIGIT(c);
    SET_V(4);
    if (v < 1900)
      HANDLE_EXCEPTION("format not supported: wrong year");
    ts.tm_year = v - 1900;
    STEP_IF_DELIMITER(c);
    STEP_DIGIT(c);
    STEP_DIGIT(c);
    SET_V(2);
    if (v == 0 || v > 12)
      HANDLE_EXCEPTION("format not supported: wrong month");
    ts.tm_mon = v - 1;
    STEP_IF_DELIMITER(c);
    STEP_DIGIT(c);
    STEP_DIGIT(c);
    SET_V(2);
    if (v == 0 || v > 31)
      HANDLE_EXCEPTION("format not supported: wrong mday");
    ts.tm_mday = v;
  } else {
    // common logfile format, HTTP format, RFC850 format 
    SET_V(2);
    if (v == 0 || v > 31)
      HANDLE_EXCEPTION("format not supported: wrong mday");
    ts.tm_mday = v;
    STEP_DELIMITER(c);
    if (*c == '\0') {
      HANDLE_EXCEPTION("format not supported");
    } else if (std::strncmp(c, "Jan", 3) == 0) {
      ts.tm_mon = 0;
    } else if (std::strncmp(c, "Feb", 3) == 0) {
      ts.tm_mon = 1;
    } else if (std::strncmp(c, "Mar", 3) == 0) {
      ts.tm_mon = 2;
    } else if (std::strncmp(c, "Apr", 3) == 0) {
      ts.tm_mon = 3;
    } else if (std::strncmp(c, "May", 3) == 0) {
      ts.tm_mon = 4;
    } else if (std::strncmp(c, "Jun", 3) == 0) {
      ts.tm_mon = 5;
    } else if (std::strncmp(c, "Jul", 3) == 0) {
      ts.tm_mon = 6;
    } else if (std::strncmp(c, "Aug", 3) == 0) {
      ts.tm_mon = 7;
    } else if (std::strncmp(c, "Sep", 3) == 0) {
      ts.tm_mon = 8;
    } else if (std::strncmp(c, "Oct", 3) == 0) {
      ts.tm_mon = 9;
    } else if (std::strncmp(c, "Nov", 3) == 0) {
      ts.tm_mon = 10;
    } else if (std::strncmp(c, "Dec", 3) == 0) {
      ts.tm_mon = 11;
    } else {
      HANDLE_EXCEPTION("format not supported: wrong month");
    }
    c += 3;
    STEP_DELIMITER(c);
    STEP_DIGIT(c);
    STEP_DIGIT(c);
    if (IS_DIGIT(*c)) {
      ++c;
      STEP_DIGIT(c);
      SET_V(4);
      if (v < 1900)
        HANDLE_EXCEPTION("format not supported: wrong year");
      ts.tm_year = v - 1900;
    } else {
      SET_V(2);
      ts.tm_year = v;
    }
  }

  if (!check_date(ts.tm_year, ts.tm_mon+1, ts.tm_mday))
    HANDLE_EXCEPTION("format not supported: invalid datetime");

  if (*c == ' ' || *c == 'T' || *c == ':')
    ++c;

  if (*c == '\0')
    return WRAP_RESULT(ts);

  STEP_RANGE(c, '0', '2');
  STEP_DIGIT(c);
  SET_V(2);
  if (v > 24)
    HANDLE_EXCEPTION("format not supported: hour is too big");
  ts.tm_hour = v;
  if (*c == ':')
    ++c;
  // found minute part
  if (*c >= '0' && *c <= '5') {
    ++c;
    STEP_DIGIT(c);
    SET_V(2);
    if (v > 59)
      HANDLE_EXCEPTION("format not supported");
    ts.tm_min = v;
    if (*c == ':')
      ++c;
  } else {
    ts.tm_min = 0;
  }
  // found second part
  if (*c >= '0' && *c <= '6') {
    ++c;
    STEP_DIGIT(c);
    SET_V(2);
    if (v > 61)
      HANDLE_EXCEPTION("format not supported");
    ts.tm_sec = v;
  } else {
    ts.tm_sec = 0;
  }
  // found floating point part, skip it
  if (*c == ',' || *c == '.') {
    ++c;
    while (IS_DIGIT(*c))
      ++c;
  }
  while (*c == ' ')
    ++c;

  // found timezone
  if (*c =='+' || *c == '-') {
    bool positive = (*c == '+') ? false : true;
    ++c;
    // get hour part of timezone bias
    STEP_RANGE(c, '0', '2');
    STEP_DIGIT(c);
    SET_V(2);
    if (v > 24)
      HANDLE_EXCEPTION("format not supported");
    if (positive)
        ts.tm_sec += v * 3600;
    else
        ts.tm_sec -= v * 3600;
    if (*c == ':')
      ++c;
    // get minutes part of timezone bias
    STEP_RANGE(c, '0', '5');
    STEP_DIGIT(c);
    SET_V(2);
    if (v > 59)
      HANDLE_EXCEPTION("format not supported");
    if (positive)
      ts.tm_sec += v * 60;
    else
      ts.tm_sec -= v * 60;
  } else if ((std::strcmp(c, "GMT") == 0)
          || (std::strcmp(c, "UTC") == 0)
          || (std::strcmp(c, "Z") == 0)) {
    ts.tm_sec += 0;
  } else if ((std::strcmp(c, "EST") == 0)
          || (std::strcmp(c, "CDT") == 0)
          || (std::strcmp(c, "E") == 0)) {
    ts.tm_sec -= 5 * 3600;
  } else if ((std::strcmp(c, "EDT") == 0)
          || (std::strcmp(c, "D") == 0)) {
    ts.tm_sec -= 4 * 3600;
  } else if ((std::strcmp(c, "CST") == 0)
          || (std::strcmp(c, "MDT") == 0)
          || (std::strcmp(c, "F") == 0)) {
    ts.tm_sec -= 6 * 3600;
  } else if ((std::strcmp(c, "MST") == 0)
          || (std::strcmp(c, "PDT") == 0)
          || (std::strcmp(c,   "G") == 0)) {
    ts.tm_sec -= 7 * 3600;
  } else if ((std::strcmp(c, "PST") == 0)
          || (std::strcmp(c, "H") == 0)) {
    ts.tm_sec -= 8 * 3600;
  } else if (std::strcmp(c, "A") == 0) {
    ts.tm_sec -= 1 * 3600;
  } else if (std::strcmp(c, "B") == 0) {
    ts.tm_sec -= 2 * 3600;
  } else if (std::strcmp(c, "C") == 0) {
    ts.tm_sec -= 3 * 3600;
  } else if (std::strcmp(c, "I") == 0) {
    ts.tm_sec -= 9 * 3600;
  } else if (std::strcmp(c, "K") == 0) {
    ts.tm_sec -= 10 * 3600;
  } else if (std::strcmp(c, "L") == 0) {
    ts.tm_sec -= 11 * 3600;
  } else if (std::strcmp(c, "M") == 0) {
    ts.tm_sec -= 12 * 3600;
  } else if (std::strcmp(c, "N") == 0) {
    ts.tm_sec += 1 * 3600;
  } else if (std::strcmp(c, "O") == 0) {
    ts.tm_sec += 2 * 3600;
  } else if (std::strcmp(c, "P") == 0) {
    ts.tm_sec += 3 * 3600;
  } else if (std::strcmp(c, "Q") == 0) {
    ts.tm_sec += 4 * 3600;
  } else if (std::strcmp(c, "R") == 0) {
    ts.tm_sec += 5 * 3600;
  } else if (std::strcmp(c, "S") == 0) {
    ts.tm_sec += 6 * 3600;
  } else if (std::strcmp(c, "T") == 0) {
    ts.tm_sec += 7 * 3600;
  } else if (std::strcmp(c, "U") == 0) {
    ts.tm_sec += 8 * 3600;
  } else if (std::strcmp(c, "V") == 0) {
    ts.tm_sec += 9 * 3600;
  } else if (std::strcmp(c, "W") == 0) {
    ts.tm_sec += 10 * 3600;
  } else if (std::strcmp(c, "X") == 0) {
    ts.tm_sec += 11 * 3600;
  } else if (std::strcmp(c, "Y") == 0) {
    ts.tm_sec += 12 * 3600;
  }
  return WRAP_RESULT(ts);
}

}  // end of namespace

#endif