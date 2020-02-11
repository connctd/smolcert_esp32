#include "smolcert.h"
#include "unity.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <sodium.h>

const uint8_t expected_cert_bytes_without_extension[] = {
        0x87, 0x0c, 0x67, 0x63, 0x6f, 0x6e, 0x6e, 0x63, 0x74, 0x64, 0x82, 0x1a, 0x5d, 0xf0, 0x2e,
        0xf1, 0x1a, 0x5d, 0xf1, 0x80, 0x71, 0x67, 0x63, 0x6f, 0x6e, 0x6e, 0x63, 0x74, 0x64, 0x58,
        0x20, 0x95, 0x38, 0xee, 0xf6, 0x5d, 0x12, 0x34, 0xa6, 0x37, 0x33, 0x45, 0x13, 0x18, 0x06,
        0xf8, 0x00, 0x6c, 0x4c, 0x6c, 0x81, 0xc8, 0xdb, 0x58, 0x19, 0x24, 0x18, 0x9f, 0x82, 0x89,
        0xdd, 0x7c, 0x43, 0x80, 0x58, 0x40, 0xd9, 0xde, 0x51, 0x67, 0x32, 0x92, 0xb3, 0xed, 0x69,
        0xaa, 0x83, 0xdd, 0xd4, 0xf2, 0x04, 0xe2, 0x5c, 0x5e, 0xd2, 0x5f, 0x7d, 0x43, 0xa0, 0x33,
        0x99, 0x0e, 0x52, 0x33, 0x9d, 0x08, 0x89, 0x77, 0xd5, 0x4c, 0x1b, 0x9d, 0x53, 0x31, 0x42,
        0x03, 0xb5, 0x1d, 0xf1, 0x38, 0x78, 0x85, 0x06, 0x87, 0xbf, 0x58, 0xe6, 0x19, 0xb0, 0xf7,
        0xa8, 0xfc, 0xd8, 0x29, 0x57, 0x90, 0x0c, 0xf7, 0x82, 0x01};

void test_Parsing_valid_smolcert(void);
void test_ValidateCertificateSignature(void);

void test_Parsing_valid_smolcert(void) {
  smolcert_t* cert = (smolcert_t*)malloc(sizeof(smolcert_t));

  sc_error_t sc_err = sc_parse_certificate((const uint8_t *)&expected_cert_bytes_without_extension, 
    sizeof(expected_cert_bytes_without_extension), cert);
  TEST_ASSERT_EQUAL(Sc_No_Error, sc_err);

  TEST_ASSERT_EQUAL_UINT64(12, cert->serial_number);
  TEST_ASSERT_EQUAL_STRING("connctd", cert->issuer);
  TEST_ASSERT_EQUAL_STRING("connctd", cert->subject);
  TEST_ASSERT_EQUAL_UINT64(1576108145, cert->validity.not_after);
  TEST_ASSERT_EQUAL_UINT64(1576021745, cert->validity.not_before);
  const uint8_t expected_pub_key[32] = {0x95, 0x38, 0xEE, 0xF6, 0x5D, 0x12, 0x34, 0xA6,
      0x37, 0x33, 0x45, 0x13, 0x18, 0x06, 0xF8, 0x00, 0x6C, 0x4C, 0x6C, 0x81,
      0xC8, 0xDB, 0x58, 0x19, 0x24, 0x18, 0x9F, 0x82, 0x89, 0xDD, 0x7C, 0x43};
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_pub_key, cert->public_key, 32);

  sc_free_cert(cert);
}

void test_ValidateCertificateSignature(void) {
  smolcert_t* cert = (smolcert_t*)malloc(sizeof(smolcert_t));

  sc_error_t sc_err = sc_parse_certificate((const uint8_t *)&expected_cert_bytes_without_extension,
    sizeof(expected_cert_bytes_without_extension), cert);
  TEST_ASSERT_EQUAL(Sc_No_Error, sc_err);

  // We can't use the const array expected_cert_bytes here, since sc_validate_certificate_signature needs
  // to temporarely modify the array. So we copy it to a new location
  uint8_t* cert_bytes = (uint8_t*)malloc(sizeof(expected_cert_bytes_without_extension));
  memcpy(cert_bytes, expected_cert_bytes_without_extension, sizeof(expected_cert_bytes_without_extension));
  sc_err = sc_validate_certificate_signature(cert_bytes, sizeof(expected_cert_bytes_without_extension), cert->public_key);
  TEST_ASSERT_EQUAL(Sc_No_Error, sc_err);

  // Ensure that the validate_signature method did not alter the original buffer
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_cert_bytes_without_extension, cert_bytes, sizeof(expected_cert_bytes_without_extension));

  free(cert_bytes);
  sc_free_cert(cert);
}

int main(void) {
    if (sodium_init() == -1) {
      return 1;
    }
    UNITY_BEGIN();
    RUN_TEST(test_Parsing_valid_smolcert);
    RUN_TEST(test_ValidateCertificateSignature);
    return UNITY_END();
}