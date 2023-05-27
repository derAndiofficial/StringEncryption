
#ifndef Encrypt_AES
#define Encrypt_AES


class Encryption {
public:

/** @param InputString: The unencryptet plain text data to be encrypted
  * @param Key: The AES256 encryption-key as a Byte[32] array
  * @param Passes: The number of encryption passes (0 = unencrypted passthrough)
  * @param return: The encrypted data using multiple AES256 encryption-cycles and random salt
  */
  String Encrypt(String InputString, const uint8_t *Key, int Passes);


  /** @param InputString: The encryptet cypher text data to be decrypted
  * @param Key: The AES256 decryption-key as a Byte[32] array
  * @param Passes: The number of decryption passes
  * @param return: The decrypted data using multiple AES256 decryption-cycles
  */
  String Decrypt(String InputString, const uint8_t *Key, int Passes);

private:

  int used_mqtt_send_variables = 0;
  int used_mqtt_receive_variables = 0;

  void splitByteArray(byte* originalArray, int originalLength, int splitLength, byte** splitArrays, int& numArrays);
  void mergeByteArrays(byte** splitArrays, int numArrays, int splitLength, byte* mergedArray);

  String addSalt(String input, const String& delimiter, byte quantity);
  String removeSalt(const String& input, const String& delimiter);

  void MultiPassEncrypt (uint8_t *input, uint8_t *output, int passes);
  void MultiPassDecrypt (uint8_t *input, uint8_t *output, int passes);

};


#endif