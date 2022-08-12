#pragma once
#include <cstdint>
#include <queue>
#include <wx/wx.h>
#include "../../nes/mappers/Cartridge.h"
#include "../wxNESStateEvent.h"
#include "../../nes/CPU/CPUInstruction.h"
#include "wxLargeUnselectableListBox.h"

class DisassemblerLineData {
public:
	DisassemblerLineData(uint16_t beginAddress, int length, InstructionMnemonic mnemonic, AddressingMode addressingMode, uint8_t dataLow, uint8_t dataHigh, size_t listIndex) :
		m_beginAddress(beginAddress), m_length(length), m_mnemonic(mnemonic), m_addressingMode(addressingMode), m_dataLow(dataLow), m_dataHigh(dataHigh), m_listIndex(listIndex) {}
	std::string ToString() const;

	uint16_t GetAddress() const;
	int GetLength() const;
	InstructionMnemonic GetMnemonic() const;
	AddressingMode GetAddressingMode() const;
	uint8_t GetDataLow() const;
	uint8_t GetDataHigh() const;
	size_t GetListIndex() const;

private:
	std::string ImpliedToString() const;
	std::string AccumulatorToString() const;
	std::string ImmediateToString() const;
	std::string ZeroPageToString() const;
	std::string AbsoluteToString() const;
	std::string RelativeToString() const;
	std::string IndirectToString() const;
	std::string ZeroPageIndexedXToString() const;
	std::string ZeroPageIndexedYToString() const;
	std::string AbsoluteIndexedXToString() const;
	std::string AbsoluteIndexedYToString() const;
	std::string IndexedIndirectXToString() const;
	std::string IndirectIndexedYToString() const;

	uint16_t m_beginAddress;
	int m_length;
	InstructionMnemonic m_mnemonic;
	AddressingMode m_addressingMode;
	uint8_t m_dataLow;
	uint8_t m_dataHigh;
	size_t m_listIndex;
};


struct DisassemblerInitializeInfo {
	DisassemblerInitializeInfo() : address(0) {}
	DisassemblerInitializeInfo(std::vector<uint8_t>& initPRGROM, std::vector<LogicalBank>& initPRGLogicalBanks, BankMapping& initPRGBankMapping, uint16_t initAddress) :
		PRGROM(initPRGROM), PRGLogicalBanks(initPRGLogicalBanks), PRGBankMapping(initPRGBankMapping), address(initAddress) {}

	std::vector<uint8_t> PRGROM;
	std::vector<LogicalBank> PRGLogicalBanks;
	BankMapping PRGBankMapping;
	uint16_t address;
};

struct DisassemblerNextStateInfo {
	DisassemblerNextStateInfo() : address(0) {}
	DisassemblerNextStateInfo(uint16_t nextAddress, BankMapping& nextBankMapping) : address(nextAddress), PRGBankMapping(nextBankMapping) {}

	uint16_t address;
	BankMapping PRGBankMapping;
};

typedef std::vector<std::vector<uint8_t>> PRGPhysicalBanks;
typedef std::vector<std::map<uint16_t, DisassemblerLineData>> DisassembledBanks;

class Disassembler {
public:
	Disassembler() : m_currAddress(0) {}
	void Initialize(DisassemblerInitializeInfo& initializationInfo);
	void Clear();
	bool SetNextState(DisassemblerNextStateInfo& nextStateInfo); // True if ListBox requires updating
	int GetCurrentAddressIndex() const;
	std::vector<std::string> GetProgramLines();

	static std::map<InstructionMnemonic, std::string> s_mnemonicToString;

private:
	void SetupPRGRom(std::vector<uint8_t>& PRGROM);
	std::map<uint16_t, DisassemblerLineData> DisassembleBank(size_t physicalBankIndex, uint16_t startAddress, size_t startIndex);
	// Returns true if there's more data to read, false if done
	bool ConsumeNextLine(std::queue<uint8_t>& restOfProgramContent, uint16_t& beginAddress, DisassemblerLineData& lineData);
	size_t GetAddressPhysicalBankIndex(uint16_t address);
	std::map<uint16_t, DisassemblerLineData> GetDisassembledDataUpToAddress(size_t physicalBankIndex, uint16_t targetAddress, size_t& lastIndex);

	PRGPhysicalBanks m_PRGROM;
	std::vector<LogicalBank> m_PRGLogicalBanks;
	uint16_t m_currAddress;
	BankMapping m_PRGBankMapping;

	DisassembledBanks m_disassembled;
};


wxDECLARE_EVENT(EVT_DISASSEMBLER_INITIALIZE, wxNESStateEvent<DisassemblerInitializeInfo>);
wxDECLARE_EVENT(EVT_DISASSEMBLER_NEXT_STATE, wxNESStateEvent<DisassemblerNextStateInfo>);


class wxDisassemblerPanel : public wxPanel {
public:
	wxDisassemblerPanel(wxWindow* parent);
	bool IsInitialized() const;

	void OnInitialize(wxNESStateEvent<DisassemblerInitializeInfo>& evt);
	void Clear();
	void OnNextState(wxNESStateEvent<DisassemblerNextStateInfo>& evt);

private:
	void UpdateList();

	Disassembler m_disassembler;
	bool m_isInitialized;

	wxLargeUnselectableListBox* m_programListBox;
};