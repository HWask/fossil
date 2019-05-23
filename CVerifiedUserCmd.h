#pragma once
#include "CUserCmd.h"
#include "CRC32.h"

class CVerifiedUserCmd
{
public:
	CUserCmd        m_cmd;
	CRC32_t         m_crc;
};