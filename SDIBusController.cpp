#ifndef __SDI_BUS_CONTROLLER_CPP
#define __SDI_BUS_CONTROLLER_CPP

// XXX: remove on product
#include <iostream>
using namespace std;

#include <stdlib.h>
#include "Arduino.h"

#include "SDIBusController.hpp"
#include "SDIRemoteSensor.hpp"

// define the singleton insance of this, will not allow anyone else to declare
// an instance of the sensor
SDIBusController SDIBus;
SDIBusError SDIBusErrno;

/* Private Members */
SDIBusController::SDIBusController() {
  mMaxSensorCount = 0;
  mSensorCount = 0;
  mFlowControlPin = -1;
}

/* Public Members */
void SDIBusController::begin(int flowControlPin, unsigned int maxSensors) {
  mFlowControlPin = flowControlPin;
  mMaxSensorCount = maxSensors;
  mSensorCount = 0;

  // allocate memory for the sensors
 // mSensors = calloc(mMaxSensors, sizeof(SDIRemoteSensor*));

    pinMode(mFlowControlPin, OUTPUT);
    Serial1.begin(1200, SERIAL_7E1);
}

void SDIBusController::end(void) {

    Serial1.end();
    digitalWrite(mFlowControlPin, 1); // buffer is receiving
}

/*
int SDIBusController::register(SDIRemoteSensor& sensor) {
  SDIBusErrno = OK;

  if (mSensorCount >= mMaxSensorCount) {
    SDIBusErrno = NO_SPACE;
    return -1;
  }

  if(this->findSensorFromAddress(sensor.getAddress()) != NULL) {
    SDIBusErrno = ADDRESS_IN_USE;
    return -1;
  }

  mSensors[mSensorCount++] = &sensor;
  return 0;
}
*/

void SDIBusController::sendPreamble() {
    // Set tri-state buffer to write mode
    setBufferWrite();

    // Stop Serial1
    Serial1.end();

    digitalWrite(mFlowControlPin, 0);
    delayMicroseconds(12000); // wait for 12 ms
    digitalWrite(mFlowControlPin, 1);
    delayMicroseconds(8300); // wait for 8.3 ms

    // re-enable Serial1
    Serial1.begin(1200, SERIAL_7E1);
}

void SDIBusController::setBufferWrite(){
    digitalWrite(mFlowControlPin, 0);
}

void SDIBusController::setBufferRead(){
    digitalWrite(mFlowControlPin, 1);
}

/*
void SDIBusController::eventLoop() {
  unsigned long currentTime = getMillis();

  for (int i = 0; i < mSensorCount; i++) {
    SDIRemoteSensor* sensorPtr = mSensor[i];

    // if the sensor is in the middle of a request and we are ready, then process
    if (sensorPtr->mBusy && currentTime >= sensorPtr->mReadyTimestamp) {
      sensorPtr->mBusy = 0;
      int status = this->getData(sensorPtr->mAddr, sensorPtr->data);
      if (status < 0) {
        continue;
      }

      sensorPtr->mAvailable = 1;
    }
  }
}
*/

int SDIBusController::addressQuery(char* outAddr) {
  sendPreamble();

  Serial1.write('?');
  Serial1.write('!');

  // TODO(colby): same as below
  while(!Serial1.available()); delay(1);
  char c0 = Serial1.read();
  while(!Serial1.available()); delay(1);
  char c1 = Serial1.read();
  while(!Serial1.available()); delay(1);
  char c2 = Serial1.read();

  if (c1 != '\r' || c2 != '\n') {
    cout << "Failure" << endl;
    SDIBusErrno = RESPONSE_ERROR;
    return -1;
  }

  *outAddr = c0;
  cout << "Success" << endl;

  SDIBusErrno = OK;
  return 0; // we made it
}

int SDIBusController::acknowledgeActive(char addr) {
  sendPreamble();

  // write data out the serial
  Serial1.write(addr);
  Serial1.write('!');

  // TODO(colby): add direction control

  // TODO(colby): add timeout handling
  while(!Serial1.available()); delay(1);
  char c0 = Serial1.read();
  while(!Serial1.available()); delay(1);
  char c1 = Serial1.read();
  while(!Serial1.available()); delay(1);
  char c2 = Serial1.read();

  if (c0 != addr || c1 != '\r' || c2 != '\n') {
    cout << "Failure" << endl;
    SDIBusErrno = RESPONSE_ERROR;
    return -1;
  }
  cout << "Success" << endl;

  SDIBusErrno = OK;
  return 0; // we made it
}

int SDIBusController::refresh(char addr, int altno) {
  return -1;
}

int SDIBusController::getData(char addr, float* buffer) {
  return -1;
}

int SDIBusController::changeAddress(char oldAddr, char newAddr) {
  return -1;
}

int SDIBusController::respondToAcknowledgeActive(char addr) {
  Serial1.write(addr);
  Serial1.write('\r');
  Serial1.write('\n');

  SDIBusErrno = OK;
  return 0;
}

#endif
