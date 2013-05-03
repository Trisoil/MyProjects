#pragma once

/**
* @file
* @brief ���� Э�� ���� ���� У�� ����
*/

namespace protocal
{
	class Cryptography
	{
	public:
		static const size_t CHECKSUM_LENGTH = 4;

		/**
		*@brief �������������ڷ���ǰ���У��
		*@param buffer ��Ҫ ��У�� ��Buffer������
		*@return �Ӽ���ɹ�������true����У��ʧ�ܣ�����false
		*/
		static bool Encrypt(Buffer buffer);

		/**
		*@brief �յ�����ʱ�����У���Ƿ���ȷ
		*@param buffer ��Ҫ ���У���Ƿ���ȷ ��Buffer������
		*@return ������ɹ�������true�����򷵻�false
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
