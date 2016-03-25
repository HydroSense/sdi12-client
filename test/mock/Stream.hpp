#ifndef __STREAM_H
#define __STREAM_H

class Stream
{
public:
  virtual int available() = 0;
  virtual int read() = 0;
  virtual int peek() = 0;
  virtual void flush() = 0;

  //From the Print class:
  virtual size_t write(int) = 0;
  virtual size_t write(const int *buf) = 0;
};

#endif
