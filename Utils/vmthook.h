#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <memory>

class VMTHook {
	private:
		std::uintptr_t** baseclass = nullptr;
		std::unique_ptr<std::uintptr_t[]> current_vft = nullptr;
		std::uintptr_t* original_vft = nullptr;
		std::size_t total_functions = 0;
	public:
		VMTHook(void) = default;

		VMTHook(void* baseclass) {
			this->baseclass = static_cast<std::uintptr_t**>(baseclass);

			while (static_cast<std::uintptr_t*>(*this->baseclass)[this->total_functions])
				++this->total_functions;

			const std::size_t table_size = this->total_functions * sizeof(std::uintptr_t);

			this->original_vft = *this->baseclass;
			this->current_vft = std::make_unique<std::uintptr_t[]>(this->total_functions);

			std::memcpy(this->current_vft.get(), this->original_vft, table_size);

			*this->baseclass = this->current_vft.get();
		};

		~VMTHook() {
			*this->baseclass = this->original_vft;
		};

		template <typename Function> inline const Function GetOriginalFunction(std::size_t function_index) {
			return reinterpret_cast<Function>(this->original_vft[function_index]);
		}

		inline bool HookFunction(void* new_function, const std::size_t function_index) {
			if (function_index > this->total_functions)
				return false;

			this->current_vft[function_index] = reinterpret_cast<std::uintptr_t>(new_function);

			return true;
		}

		inline bool UnhookFunction(const std::size_t function_index) {
			if (function_index > this->total_functions)
				return false;

			this->current_vft[function_index] = this->original_vft[function_index];

			return true;
		}

		inline std::size_t GetTotalFunctions() {
			return this->total_functions;
		}
};

class CVMTHookManager
{
public:
	CVMTHookManager(void)
	{
		memset(this, 0, sizeof(CVMTHookManager));
	}

	CVMTHookManager(PDWORD* ppdwClassBase)
	{
		bInitialize(ppdwClassBase);
	}

	~CVMTHookManager(void)
	{
		UnHook();
	}
	bool bInitialize(PDWORD* ppdwClassBase)
	{
		m_ppdwClassBase = ppdwClassBase;
		m_pdwOldVMT = *ppdwClassBase;
		m_dwVMTSize = dwGetVMTCount(*ppdwClassBase);
		m_pdwNewVMT = new DWORD[m_dwVMTSize];
		memcpy(m_pdwNewVMT, m_pdwOldVMT, sizeof(DWORD) * m_dwVMTSize);
		*ppdwClassBase = m_pdwNewVMT;
		return true;
	}
	bool bInitialize(PDWORD** pppdwClassBase) // fix for pp
	{
		return bInitialize(*pppdwClassBase);
	}

	void UnHook(void)
	{
		if (m_ppdwClassBase)
		{
			*m_ppdwClassBase = m_pdwOldVMT;
		}
	}

	void ReHook(void)
	{
		if (m_ppdwClassBase)
		{
			*m_ppdwClassBase = m_pdwNewVMT;
		}
	}

	int iGetFuncCount(void)
	{
		return (int)m_dwVMTSize;
	}

	DWORD dwGetMethodAddress(int Index)
	{
		if (Index >= 0 && Index <= (int)m_dwVMTSize && m_pdwOldVMT != NULL)
		{
			return m_pdwOldVMT[Index];
		}
		return NULL;
	}

	PDWORD pdwGetOldVMT(void)
	{
		return m_pdwOldVMT;
	}

	DWORD dwHookMethod(DWORD dwNewFunc, unsigned int iIndex)
	{
		if (m_pdwNewVMT && m_pdwOldVMT && iIndex <= m_dwVMTSize && iIndex >= 0)
		{
			m_pdwNewVMT[iIndex] = dwNewFunc;
			return m_pdwOldVMT[iIndex];
		}

		return NULL;
	}

private:
	DWORD dwGetVMTCount(PDWORD pdwVMT)
	{
		DWORD dwIndex = 0;

		for (dwIndex = 0; pdwVMT[dwIndex]; dwIndex++)
		{
			if (IsBadCodePtr((FARPROC)pdwVMT[dwIndex]))
			{
				break;
			}
		}
		return dwIndex;
	}
	PDWORD*	m_ppdwClassBase;
	PDWORD	m_pdwNewVMT, m_pdwOldVMT;
	DWORD	m_dwVMTSize;
};