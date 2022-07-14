#pragma once
#include <cstdint>
#include <queue>
#include <wx/wx.h>
#include "../wxMainFrame.h"
#include "../wxNESStateEvent.h"
#include "../../nes/NES.h"
#include "../../nes/CPU/CPUInstruction.h"
#include "wxLargeUnselectableListBox.h"
class wxMainFrame;

class DisassemblerLineData {
public:
	DisassemblerLineData(uint16_t beginAddress, int length, InstructionMnemonic mnemonic, AddressingMode addressingMode, uint8_t dataLow, uint8_t dataHigh) :
		m_beginAddress(beginAddress), m_length(length), m_mnemonic(mnemonic), m_addressingMode(addressingMode), m_dataLow(dataLow), m_dataHigh(dataHigh) {}
	std::string ToString();

	uint16_t GetAddress() const;
	int GetLength() const;
	InstructionMnemonic GetMnemonic() const;
	AddressingMode GetAddressingMode() const;
	uint8_t GetDataLow() const;
	uint8_t GetDataHigh() const;

private:
	std::string ImpliedToString();
	std::string AccumulatorToString();
	std::string ImmediateToString();
	std::string ZeroPageToString();
	std::string AbsoluteToString();
	std::string RelativeToString();
	std::string IndirectToString();
	std::string ZeroPageIndexedXToString();
	std::string ZeroPageIndexedYToString();
	std::string AbsoluteIndexedXToString();
	std::string AbsoluteIndexedYToString();
	std::string IndexedIndirectXToString();
	std::string IndirectIndexedYToString();

	uint16_t m_beginAddress;
	int m_length;
	InstructionMnemonic m_mnemonic;
	AddressingMode m_addressingMode;
	uint8_t m_dataLow;
	uint8_t m_dataHigh;
};

class Disassembler {
public:
	Disassembler() : m_currAddress(0) {}
	void Initialize(std::vector<uint8_t> programContent, uint16_t beginAddress, uint16_t endAddress, uint16_t currAddress);
	void Clear();
	void SetNextAddress(uint16_t newAddress);
	int GetCurrentAddressIndex() const;
	std::vector<std::string> GetProgramLines();

	static std::map<InstructionMnemonic, std::string> s_mnemonicToString;
	static std::map<AddressingMode, int> s_addressingModeToLength;

private:
	// Returns true if there's more data to read, false if done
	bool ConsumeNextLine(std::queue<uint8_t>& restOfProgramContent, uint16_t& beginAddress, DisassemblerLineData& lineData);

	std::vector<DisassemblerLineData> m_programData;
	std::unordered_map<uint16_t, int> m_addressToIndex;
	uint16_t m_currAddress;
};

struct DisassemblerInitializeInfo {
	DisassemblerInitializeInfo() = default;
	DisassemblerInitializeInfo(std::vector<uint8_t> content, uint16_t bAddress, uint16_t eAddress, uint16_t cAddress) :
		programContent(content), beginAddress(bAddress), endAddress(eAddress), currAddress(cAddress) {}

	std::vector<uint8_t> programContent;
	uint16_t beginAddress;
	uint16_t endAddress;
	uint16_t currAddress;
};


wxDECLARE_EVENT(EVT_DISASSEMBLER_INITIALIZE, wxNESStateEvent<DisassemblerInitializeInfo>);
wxDECLARE_EVENT(EVT_DISASSEMBLER_NEXT_ADDRESS, wxNESStateEvent<uint16_t>);


class wxDisassemblerPanel : public wxPanel {
public:
	wxDisassemblerPanel(wxWindow* parent);
	bool IsInitialized() const;

	void OnInitialize(wxNESStateEvent<DisassemblerInitializeInfo>& evt);
	void Clear();
	void OnNextAddress(wxNESStateEvent<uint16_t>& evt);

private:
	wxMainFrame* m_mainFrame;

	Disassembler m_disassembler;
	bool m_isInitialized;

	wxLargeUnselectableListBox* m_programListBox;
};