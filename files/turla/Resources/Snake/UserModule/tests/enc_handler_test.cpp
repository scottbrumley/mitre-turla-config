#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "enc_handler.h"
#include "test_util.h"

#define DATA_LEN 445
#define IV_LEN CryptoPP::CAST128::BLOCKSIZE
#define LOREM_IPSUM "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

using ::testing::Return;


// Text fixture for shared data
class EncryptionTest : public ::testing::Test {
protected:
    MockApiWrapper mock_api_wrapper;
    const unsigned char* dummy_data = (const unsigned char*)LOREM_IPSUM;
    const unsigned char expected_ciphertext[DATA_LEN] = {0x7d,0x09,0x4b,0x55,0x5e,0x10,0x5c,0x43,0x19,0x19,0x0b,0x41,0x0e,0x1c,0x07,0x03,0x18,0x13,0x4a,0x59,0x45,0x19,0x51,0x5c,0x56,0x01,0x58,0x10,0x5a,0x57,0x0b,0x44,0x52,0x1b,0x1c,0x09,0x15,0x1f,0x19,0x4c,0x10,0x14,0x06,0x0a,0x1c,0x15,0x0c,0x0e,0x06,0x0e,0x16,0x51,0x5e,0x59,0x4d,0x14,0x43,0x11,0x08,0x00,0x41,0x0f,0x0e,0x4e,0x01,0x18,0x1c,0x1c,0x15,0x5a,0x57,0x16,0x4c,0x5c,0x55,0x1a,0x06,0x03,0x4a,0x19,0x0b,0x55,0x59,0x5d,0x5b,0x17,0x18,0x03,0x1f,0x45,0x00,0x00,0x10,0x5e,0x58,0x52,0x59,0x43,0x03,0x19,0x55,0x47,0x10,0x51,0x5c,0x06,0x03,0x14,0x04,0x4a,0x1e,0x0a,0x0b,0x04,0x52,0x19,0x51,0x5d,0x50,0x41,0x44,0x52,0x5b,0x54,0x65,0x4d,0x18,0x00,0x59,0x5e,0x15,0x48,0x0d,0x05,0x4a,0x06,0x05,0x1f,0x19,0x02,0x5a,0x03,0x03,0x01,0x0e,0x09,0x04,0x1a,0x14,0x43,0x45,0x50,0x4b,0x43,0x0c,0x02,0x17,0x15,0x19,0x14,0x0a,0x44,0x14,0x11,0x0a,0x0a,0x56,0x5a,0x42,0x59,0x4d,0x51,0x05,0x07,0x51,0x1f,0x1c,0x09,0x57,0x5d,0x5a,0x5d,0x53,0x01,0x0c,0x09,0x0a,0x07,0x1d,0x43,0x12,0x57,0x59,0x45,0x58,0x46,0x4c,0x44,0x13,0x51,0x59,0x5a,0x1b,0x19,0x0f,0x11,0x4a,0x16,0x13,0x4c,0x0f,0x52,0x19,0x53,0x5e,0x54,0x5d,0x5e,0x57,0x1a,0x54,0x53,0x56,0x56,0x16,0x52,0x46,0x0d,0x09,0x18,0x4f,0x4a,0x2f,0x19,0x18,0x03,0x4f,0x1b,0x00,0x12,0x0a,0x47,0x01,0x1b,0x43,0x46,0x57,0x10,0x5d,0x57,0x0f,0x0d,0x1f,0x44,0x08,0x05,0x41,0x1c,0x01,0x01,0x1b,0x0a,0x10,0x50,0x5d,0x52,0x5d,0x4b,0x51,0x1e,0x49,0x18,0x04,0x50,0x13,0x59,0x5c,0x4c,0x42,0x07,0x0c,0x19,0x0e,0x45,0x03,0x11,0x5c,0x5b,0x4d,0x10,0x53,0x42,0x15,0x5c,0x10,0x50,0x59,0x59,0x5f,0x1f,0x01,0x46,0x05,0x05,0x1f,0x04,0x1e,0x0f,0x13,0x5c,0x45,0x11,0x5f,0x45,0x56,0x5a,0x14,0x00,0x10,0x57,0x4d,0x09,0x5b,0x56,0x58,0x18,0x0d,0x13,0x03,0x0a,0x18,0x04,0x02,0x41,0x5a,0x30,0x1e,0x0c,0x02,0x18,0x1d,0x53,0x41,0x40,0x10,0x4a,0x51,0x0d,0x16,0x4d,0x0b,0x02,0x08,0x00,0x0b,0x07,0x10,0x1d,0x4f,0x1b,0x40,0x43,0x5f,0x5c,0x58,0x4c,0x0b,0x1d,0x51,0x04,0x1f,0x0b,0x16,0x40,0x4b,0x5d,0x1a,0x09,0x08,0x05,0x11,0x59,0x54,0x43,0x47,0x57,0x44,0x16,0x58,0x08,0x19,0x53,0x46,0x5c,0x45,0x52,0x4a,0x1d,0x13,0x08,0x4a,0x1c,0x0d,0x0a,0x03,0x50,0x50,0x51,0x11,0x5d,0x55,0x42,0x56,0x07,0x01,0x5e,0x4d,0x18,0x08,0x58,0x5b,0x14,0x01,0x18,0x41,0x0b,0x05,0x05,0x1c,0x50,0x06,0x1e,0x55,0x03,0x1c,0x13,0x48,0x05,0x57,0x56,0x5d,0x42,0x4c,0x55,0x4d};
    std::string key_password = "password";
    std::string key_salt = "salt";
    std::string dummy_data_str = std::string(LOREM_IPSUM);
    std::vector<char> dummy_cast_plaintext = std::vector<char>(dummy_data_str.begin(), dummy_data_str.end());
    std::vector<char> dummy_cast_plaintext2 = std::vector<char>(dummy_data_str.begin(), dummy_data_str.end());
    std::vector<CryptoPP::byte> mock_iv = std::vector<CryptoPP::byte>(IV_LEN);
    std::vector<CryptoPP::byte> mock_iv2 = std::vector<CryptoPP::byte>(IV_LEN);

    // echo -n "lorem ipsum text" | openssl enc -e -cast5-cbc -iv "00000000" -K "0123456789abcdef0123456789abcdef" | xxd -p -i -c 10000
    std::vector<int> cast_ciphertext_int = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // IV
        0xd6, 0xeb, 0xc0, 0x1b, 0x00, 0x95, 0x85, 0x3f, 0x7b, 0xa1, 0x49, 0x65, 0x88, 0x07, 0x4e, 0x1f, 0x5c, 0x3d, 0x6d, 0xd3, 0xb3, 0x63, 0xcf, 0x78, 0x94, 0x8e, 0x0c, 0xac, 0x70, 0x8e, 0x4c, 0x82, 0x23, 0xb8, 0x00, 0xcb, 0x50, 0x3a, 0x90, 0x61, 0xf2, 0xd6, 0xea, 0x17, 0xa9, 0xe7, 0x66, 0x87, 0xc9, 0x6c, 0xf2, 0x1f, 0x22, 0x89, 0x44, 0xe7, 0x2a, 0x61, 0x8d, 0xd5, 0xef, 0xf4, 0x8f, 0x52, 0x8f, 0x35, 0xdd, 0xaf, 0xee, 0xbe, 0xd6, 0x0f, 0xf8, 0x22, 0x46, 0xbd, 0xce, 0x44, 0x14, 0xa5, 0xc2, 0x02, 0x58, 0xd7, 0xf9, 0x9d, 0xfe, 0x3f, 0xe2, 0x46, 0x54, 0x76, 0x37, 0x4b, 0xa3, 0xc6, 0x9f, 0x22, 0x05, 0xc2, 0x10, 0xe8, 0xac, 0x57, 0x77, 0x6f, 0xf5, 0xc0, 0x14, 0x98, 0x44, 0xbf, 0x29, 0x96, 0x44, 0x65, 0xde, 0xf3, 0x4d, 0xe4, 0x37, 0x61, 0x7e, 0x13, 0x52, 0x67, 0xf1, 0xef, 0x62, 0x29, 0x52, 0x75, 0xb0, 0x86, 0x41, 0x67, 0x95, 0xb5, 0x7a, 0x3d, 0xaf, 0xe5, 0xdb, 0xe8, 0xea, 0x67, 0x2f, 0x85, 0xec, 0xfa, 0xdd, 0x6c, 0x58, 0xe4, 0xe9, 0x00, 0xd6, 0x25, 0xe7, 0xfd, 0xc6, 0x95, 0x98, 0x0f, 0x71, 0xac, 0xef, 0x3d, 0x3d, 0x31, 0xd1, 0xb7, 0x8d, 0x15, 0xa9, 0x2c, 0x4e, 0xce, 0xa4, 0xa1, 0x03, 0x2f, 0x3a, 0x25, 0x4f, 0xf1, 0xb9, 0x7d, 0xc7, 0x0f, 0xfa, 0xa1, 0x9c, 0x73, 0x51, 0xc9, 0x9e, 0xf8, 0xbd, 0xb9, 0xfe, 0x64, 0x95, 0xf1, 0xf0, 0x90, 0x5d, 0x53, 0x24, 0x8c, 0x2c, 0x07, 0xc3, 0x22, 0x9a, 0xc3, 0x9c, 0xff, 0xec, 0x68, 0xde, 0x01, 0xbe, 0xbb, 0x82, 0xfd, 0x09, 0x45, 0x14, 0x96, 0x22, 0x35, 0xec, 0x83, 0xb3, 0x35, 0xf9, 0x51, 0xac, 0x3f, 0x57, 0x53, 0xb0, 0x08, 0x40, 0xf9, 0x35, 0x75, 0xd1, 0x1d, 0x85, 0x20, 0xc1, 0x2f, 0x5d, 0xfe, 0x23, 0x91, 0x72, 0x06, 0xbe, 0x5b, 0x97, 0x05, 0x59, 0x14, 0xf2, 0x38, 0xac, 0xb4, 0x4f, 0x59, 0x84, 0x59, 0xbf, 0x93, 0x12, 0xfe, 0x1b, 0xbd, 0x90, 0xb3, 0x9e, 0x2f, 0x78, 0x9b, 0x3c, 0xd7, 0x0f, 0xc7, 0xf4, 0x7c, 0x49, 0x34, 0x31, 0x02, 0x52, 0x0b, 0xde, 0x70, 0x70, 0xa6, 0xd3, 0x55, 0x95, 0x4b, 0xa4, 0x32, 0xcc, 0xca, 0x5f, 0x5d, 0xa8, 0xae, 0x1f, 0x32, 0x75, 0xe6, 0x69, 0xb9, 0x67, 0x66, 0x70, 0x84, 0xb1, 0xf1, 0x75, 0x00, 0x24, 0xd0, 0x21, 0x87, 0xfe, 0x4f, 0x08, 0xf6, 0xcb, 0xf9, 0x2b, 0xe1, 0xf6, 0x78, 0xd1, 0xa7, 0x71, 0x1c, 0xbd, 0x31, 0x33, 0x06, 0x39, 0x6d, 0xe7, 0x1e, 0x89, 0x2e, 0xf0, 0x60, 0x5f, 0xb9, 0xfc, 0x88, 0x7f, 0xbc, 0xdb, 0xb3, 0xa2, 0x36, 0xf9, 0xaf, 0x55, 0x8f, 0x9a, 0x49, 0x51, 0xcc, 0xf1, 0x08, 0x98, 0xbd, 0xc2, 0x54, 0xc9, 0xb2, 0xb5, 0xc4, 0x14, 0xb7, 0x89, 0x8d, 0x47, 0x2a, 0x4b, 0x32, 0xb6, 0x48, 0xa7, 0x49, 0x72, 0xc8, 0xd9, 0xe7, 0x09, 0x4e, 0x52, 0xbc, 0x9e, 0xfd, 0x1d, 0xda, 0x41, 0x50, 0x20, 0x3f, 0x70, 0xb8, 0x2d, 0xb2, 0x34, 0x52, 0xd2, 0x44, 0x4d, 0x2b, 0xd5, 0x3b, 0xbb, 0x07, 0xcf, 0xe1, 0x7e, 0x2b, 0xb5, 0x2a, 0xb5, 0x58, 0x87, 0x66, 0xb8, 0x4f, 0x51, 0x6d, 0x8b, 0xe0, 0x88, 0x27, 0xeb, 0x91};

    std::vector<char> expected_cast_ciphertext = std::vector<char>(cast_ciphertext_int.size());

    // https://gchq.github.io/CyberChef/#recipe=Derive_PBKDF2_key(%7B'option':'UTF8','string':'password'%7D,128,5,'SHA1',%7B'option':'UTF8','string':'salt'%7D)
    std::vector<unsigned char> expected_key = {
        (unsigned char)0x1a, (unsigned char)0x61, (unsigned char)0x92, (unsigned char)0xe5, 
        (unsigned char)0x3a, (unsigned char)0x6f, (unsigned char)0x18, (unsigned char)0x67, 
        (unsigned char)0x3c, (unsigned char)0xf3, (unsigned char)0xc0, (unsigned char)0xe7, 
        (unsigned char)0x07, (unsigned char)0xd3, (unsigned char)0x30, (unsigned char)0xe7
    };

    std::vector<unsigned char> dummy_cast_key = { 
        (unsigned char)0x01, (unsigned char)0x23, (unsigned char)0x45, (unsigned char)0x67, 
        (unsigned char)0x89, (unsigned char)0xab, (unsigned char)0xcd, (unsigned char)0xef, 
        (unsigned char)0x01, (unsigned char)0x23, (unsigned char)0x45, (unsigned char)0x67, 
        (unsigned char)0x89, (unsigned char)0xab, (unsigned char)0xcd, (unsigned char)0xef
    };

    void SetUp() override {
        dummy_cast_plaintext2.insert(dummy_cast_plaintext2.end(), dummy_data_str.begin(), dummy_data_str.end());

        for (int i = 0; i < cast_ciphertext_int.size(); i++) {
            expected_cast_ciphertext[i] = (char)cast_ciphertext_int[i];
        }

        for (int i = 0; i < IV_LEN; i++) {
            mock_iv[i] = (CryptoPP::byte)0;
            mock_iv2[i] = (CryptoPP::byte)i;
        }
    }
};

TEST_F(EncryptionTest, TestXorInPlace) {
    unsigned char buffer[DATA_LEN];
    memcpy(buffer, dummy_data, DATA_LEN);
    XorInPlace(buffer, DATA_LEN);
    ASSERT_EQ(memcmp(buffer, expected_ciphertext, DATA_LEN), 0);

    XorInPlace(buffer, DATA_LEN);
    ASSERT_EQ(memcmp(buffer, dummy_data, DATA_LEN), 0);
}

TEST_F(EncryptionTest, TestGenerateCast128Key) {
    ASSERT_EQ(enc_handler::GenerateCast128Key(key_password, key_salt), expected_key);
}

TEST_F(EncryptionTest, TestCast128Encrypt) {
    EXPECT_CALL(mock_api_wrapper, GenerateIvWrapper(IV_LEN)).Times(1).WillOnce(Return(mock_iv));
    ASSERT_EQ(enc_handler::Cast128Encrypt(&mock_api_wrapper, dummy_cast_plaintext, dummy_cast_key), expected_cast_ciphertext);
}

TEST_F(EncryptionTest, TestCast128Deccrypt) {
    ASSERT_EQ(enc_handler::Cast128Decrypt(expected_cast_ciphertext, dummy_cast_key), dummy_cast_plaintext);
}

TEST_F(EncryptionTest, TestCast128EncryptDeccrypt) {
    EXPECT_CALL(mock_api_wrapper, GenerateIvWrapper(IV_LEN)).Times(1).WillOnce(Return(mock_iv));

    std::vector<char> ciphertext = enc_handler::Cast128Encrypt(&mock_api_wrapper, dummy_cast_plaintext2, dummy_cast_key);
    ASSERT_EQ(enc_handler::Cast128Decrypt(ciphertext, dummy_cast_key), dummy_cast_plaintext2);
}