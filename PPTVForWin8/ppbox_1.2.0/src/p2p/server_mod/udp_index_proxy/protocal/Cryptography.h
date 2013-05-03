#pragma once

/**
* @file
* @brief 用于 协议 加密 或者 校验 的类
*/

namespace protocal
{
	class Cryptography
	{
	public:
		static const size_t CHECKSUM_LENGTH = 4;

		/**
		*@brief 给缓冲区内容在发包前添加校验
		*@param buffer 需要 加校验 的Buffer缓冲区
		*@return 加检验成功，返回true，加校验失败，返回false
		*/
		static bool Encrypt(Buffer buffer);

		/**
		*@brief 收到数据时，检查校验是否正确
		*@param buffer 需要 检查校验是否正确 的Buffer缓冲区
		*@return 检查检验成功，返回true，否则返回false
		*/
		static bool Decrypt(Buffer buffer);

	private:
		// Encrypt/Decrypt
		static const size_t ENCRYPT_ROUNDS = 32; // at least 32
		static const u_int DELTA = 0x9E3779B9;
		static const u_int FINAL_SUM = 0xC6EF3720;
	private:
		// Check Sum
		static const size_t BLOCK_SIZE = (sizeof(u_int) << 1);
		static const size_t BLOCK_SIZE_TWICE = (BLOCK_SIZE << 1);
		static const size_t BLOCK_SIZE_HALF = (BLOCK_SIZE >> 1);
	private:
		inline static bool CheckSum(Buffer buffer, bool is_override);
		inline static void TGetKey(Buffer buffer, u_int& k0, u_int& k1, u_int& k2, u_int& k3);
		inline static void TEncrypt(Buffer buffer);
		inline static void TDecrypt(Buffer buffer);
	};
}
