
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "StringEncryption.h"



void StringEncryption_AES::setup(const uint8_t *Key, uint8_t keyLength) { 
  aes256.setKey(Key, keyLength);
}




bool StringEncryption_AES::EncryptString(String &InputString, String &OutputString, short length) {

  const unsigned int MAX_SEGMENT_LENGTH = 14;  // 14 charaters from original String + delimiter + salt

  OutputString = "";  // Clear the output
  

  // Split input into segments, add delimiter & salt, encrypt and add to output String 
  for (unsigned int i = 0; i < InputString.length(); i += MAX_SEGMENT_LENGTH) {

    // Get segment of input string
    String string_segment = InputString.substring(i, min((i + MAX_SEGMENT_LENGTH), InputString.length()));

    addSalt:

    // Copy segment to string (so string_segment stays the same in case of needed encryption redo)
    String String16 = string_segment;

    // Add delimiter to string
    String16 += DELIMITER;

    // Add salt to fill the remaining space up to 16 characters
    char salt;
    while (String16.length() < 16) {
      salt = char(random(1, 255));
      if (salt != DELIMITER) {  // Only add the salt when it's not the delimiter
        String16 += salt;
      }
    }

    // Convert to byte array
    uint8_t Byte16[16];
    for (int j = 0; j < 16; j++) {    
      Byte16[j] = (uint8_t)String16[j];
    }

    // Encrypt the Byte[16] block
    aes256.encryptBlock(Byte16, Byte16);

    // Check for NULL bytes (else redo salt)
    for (int j = 0; j < 16; j++) {
      if (Byte16[j] == 0)
        goto addSalt;
    }

    // Convert back to String
    String String16_output = "";
    for (int j = 0; j < 16; j++) {
      String16_output += (char)Byte16[j];
    }
    
    // Add substring to output
    OutputString += String16_output;
  }
  
  return true;
}




bool StringEncryption_AES::DecryptString(String &InputString, String &OutputString, short length) {

  const unsigned int SEGMENT_LENGTH = 16;

  OutputString = "";
  
  // Split the input string into 16 char segments
  for (unsigned int i = 0; i < InputString.length(); i += SEGMENT_LENGTH) {

    // Get segment of input string
    String String16 = InputString.substring(i, (i + SEGMENT_LENGTH));

    // Convert to byte-array
    uint8_t Byte16[16];
    for (int j = 0; j < 16; j++) {    
      Byte16[j] = (uint8_t)String16[j];
    }

    // Decrypt
    aes256.decryptBlock(Byte16, Byte16);

    // Convert back to String
    String out_segment = "";
    for (int j = 0; j < 16; j++) {
      out_segment += (char)Byte16[j];
    }

    // Remove the salt at the delimiter
    // First look an index 14 (character 15) if not at 14 decrement search-index
    int delimiterIndex = 14;
    if (out_segment[delimiterIndex] != DELIMITER) {
      
      for (int i = delimiterIndex; i >= 0; i--) {
        if (out_segment[i] == DELIMITER) {
          delimiterIndex = i;
          break;
        }
      }
    }

    // Add substring to output
    OutputString += out_segment.substring(0, delimiterIndex);
  }

  return true;
}













void StringEncryption_ChaCha::setup(const uint8_t *Key, uint8_t keyLength) { 
  chacha.setKey(Key, keyLength);
  chacha.setCounter(ChaChaCounter, 8);
}




bool StringEncryption_ChaCha::EncryptString(String &InputString, String &OutputString, short length) {
  OutputString = "";  // Clear the output
  

  // Generate random 8 Byte nonce and add it to the front of the output
  uint8_t Nonce[8];
  for (uint8_t i = 0; i < 8; i++) {
    Nonce[i] = random(1, 255);
    OutputString += char(Nonce[i]);
  }

  chacha.setIV(Nonce, 8);


  // Convert input-string to byte-array
  unsigned char OutByteArr[length];
  InputString.getBytes(OutByteArr, length+1);

  // Encrypt
  chacha.encrypt(OutByteArr, OutByteArr, length);

  // Convert back to String
  for (int j = 0; j < length; j++) {
    OutputString += (char)OutByteArr[j];
  }

  return true;
}




bool StringEncryption_ChaCha::DecryptString(String &InputString, String &OutputString, short length) {
    OutputString = ""; // Clear the output
    short CypherLength = length - 8;


    // Get nonce from InputString (the first 8 characters)
    uint8_t Nonce[8];
    for (int i = 0; i < 8; i++) {
        Nonce[i] = InputString[i];
    }

    chacha.setIV(Nonce, 8);


    // Convert input to byte array
    unsigned char OutByteArr[CypherLength];
    for (short i = 0; i < CypherLength; i++) {
        OutByteArr[i] = InputString[i+8]; // Start from index 8
    }

    // Decrypt
    chacha.decrypt(OutByteArr, OutByteArr, CypherLength);

    // Convert back to String
    for (int j = 0; j < CypherLength; j++) {
      OutputString += (char)OutByteArr[j];
    }

    return true;
}