#include <cstdint>

#include "wxDisassemblerPanel.h"
#include "../../utils/BitwiseUtils.h"
#include "../../utils/NESUtils.h"
#include "../../nes/CPU/CPU2A03.h"

using namespace BitwiseUtils;
using namespace NESUtils;

std::map<InstructionMnemonic, std::string> Disassembler::s_mnemonicToString = {
	{INSTR_ADC, "ADC"},
	{INSTR_AND, "AND"},
	{INSTR_ASL, "ASL"},
	{INSTR_BCC, "BCC"},
	{INSTR_BCS, "BCS"},
	{INSTR_BEQ, "BEQ"},
	{INSTR_BIT, "BIT"},
	{INSTR_BMI, "BMI"},
	{INSTR_BNE, "BNE"},
	{INSTR_BPL, "BPL"},
	{INSTR_BRK, "BRK"},
	{INSTR_BVC, "BVC"},
	{INSTR_BVS, "BVS"},
	{INSTR_CLC, "CLC"},
	{INSTR_CLD, "CLD"},
	{INSTR_CLI, "CLI"},
	{INSTR_CLV, "CLV"},
	{INSTR_CMP, "CMP"},
	{INSTR_CPX, "CPX"},
	{INSTR_CPY, "CPY"},
	{INSTR_DEC, "DEC"},
	{INSTR_DEX, "DEX"},
	{INSTR_DEY, "DEY"},
	{INSTR_EOR, "EOR"},
	{INSTR_INC, "INC"},
	{INSTR_INX, "INX"},
	{INSTR_INY, "INY"},
	{INSTR_JMP, "JMP"},
	{INSTR_JSR, "JSR"},
	{INSTR_LDA, "LDA"},
	{INSTR_LDX, "LDX"},
	{INSTR_LDY, "LDY"},
	{INSTR_LSR, "LSR"},
	{INSTR_NOP, "NOP"},
	{INSTR_ORA, "ORA"},
	{INSTR_PHA, "PHA"},
	{INSTR_PHP, "PHP"},
	{INSTR_PLA, "PLA"},
	{INSTR_PLP, "PLP"},
	{INSTR_ROL, "ROL"},
	{INSTR_ROR, "ROR"},
	{INSTR_RTI, "RTI"},
	{INSTR_RTS, "RTS"},
	{INSTR_SBC, "SBC"},
	{INSTR_SEC, "SEC"},
	{INSTR_SED, "SED"},
	{INSTR_SEI, "SEI"},
	{INSTR_STA, "STA"},
	{INSTR_STX, "STX"},
	{INSTR_STY, "STY"},
	{INSTR_TAX, "TAX"},
	{INSTR_TAY, "TAY"},
	{INSTR_TSX, "TSX"},
	{INSTR_TXA, "TXA"},
	{INSTR_TXS, "TXS"},
	{INSTR_TYA, "TYA"},
	{INSTR_ILLEGAL, "ILLEGAL"}
};

DisassemblerLineData::DisassemblerLineData() {
	m_beginAddress = 0;
	m_length = 0;
	m_mnemonic = INSTR_BRK;
	m_addressingMode = MODE_IMPLIED;
	m_dataLow = 0;
	m_dataHigh = 0;
	m_listIndex = 0;
}


std::string DisassemblerLineData::ToString(uint16_t addressOffset) const {
	std::string result;
	result += HexUint16ToString(m_beginAddress + addressOffset) + ": ";
	result += Disassembler::s_mnemonicToString[m_mnemonic];

	switch (m_addressingMode) {
		case MODE_IMPLIED:
			result += this->ImpliedToString();
			break;
		case MODE_ACCUMULATOR:
			result += this->AccumulatorToString();
			break;
		case MODE_IMMEDIATE:
			result += this->ImmediateToString();
			break;
		case MODE_ZERO_PAGE:
			result += this->ZeroPageToString();
			break;
		case MODE_ABSOLUTE:
			result += this->AbsoluteToString();
			break;
		case MODE_RELATIVE:
			result += this->RelativeToString(addressOffset);
			break;
		case MODE_INDIRECT:
			result += this->IndirectToString();
			break;
		case MODE_ZERO_PAGE_INDEXED_X:
			result += this->ZeroPageIndexedXToString();
			break;
		case MODE_ZERO_PAGE_INDEXED_Y:
			result += this->ZeroPageIndexedYToString();
			break;
		case MODE_ABSOLUTE_INDEXED_X:
			result += this->AbsoluteIndexedXToString();
			break;
		case MODE_ABSOLUTE_INDEXED_Y:
			result += this->AbsoluteIndexedYToString();
			break;
		case MODE_INDEXED_INDIRECT_X:
			result += this->IndexedIndirectXToString();
			break;
		case MODE_INDIRECT_INDEXED_Y:
			result += this->IndirectIndexedYToString();
			break;
	}

	return result;
}

uint16_t DisassemblerLineData::GetAddress() const {
	return m_beginAddress;
}

int DisassemblerLineData::GetLength() const {
	return m_length;
}

InstructionMnemonic DisassemblerLineData::GetMnemonic() const {
	return m_mnemonic;
}

AddressingMode DisassemblerLineData::GetAddressingMode() const {
	return m_addressingMode;
}

uint8_t DisassemblerLineData::GetDataLow() const {
	return m_dataLow;
}

uint8_t DisassemblerLineData::GetDataHigh() const {
	return m_dataHigh;
}

size_t DisassemblerLineData::GetListIndex() const {
	return m_listIndex;
}

std::string DisassemblerLineData::ImpliedToString() const
{
	return "";
}

std::string DisassemblerLineData::AccumulatorToString() const
{
	return "";
}

std::string DisassemblerLineData::ImmediateToString() const
{
	return " #$" + HexUint8ToString(m_dataLow);
}

std::string DisassemblerLineData::ZeroPageToString() const
{
	return " $" + HexUint8ToString(m_dataLow);
}

std::string DisassemblerLineData::AbsoluteToString() const
{
	return " $" + HexUint16ToString(CombineBytes(m_dataLow, m_dataHigh));
}

std::string DisassemblerLineData::RelativeToString(uint16_t addressOffset) const
{
	uint16_t resultAddress = Add16Bit(m_beginAddress + addressOffset, m_length);
	resultAddress = AddRelativeAddress(resultAddress, m_dataLow);
	return " $" + HexUint8ToString(m_dataLow) + " [$" + HexUint16ToString(resultAddress) + "]";
}

std::string DisassemblerLineData::IndirectToString() const
{
	return " ($" + HexUint16ToString(CombineBytes(m_dataLow, m_dataHigh)) + ")";
}

std::string DisassemblerLineData::ZeroPageIndexedXToString() const
{
	return " $" + HexUint8ToString(m_dataLow) + ",X";
}

std::string DisassemblerLineData::ZeroPageIndexedYToString() const
{
	return " $" + HexUint8ToString(m_dataLow) + ",Y";
}

std::string DisassemblerLineData::AbsoluteIndexedXToString() const
{
	return " $" + HexUint16ToString(CombineBytes(m_dataLow, m_dataHigh)) + ",X";
}

std::string DisassemblerLineData::AbsoluteIndexedYToString() const
{
	return " $" + HexUint16ToString(CombineBytes(m_dataLow, m_dataHigh)) + ",Y";
}

std::string DisassemblerLineData::IndexedIndirectXToString() const
{
	return " ($" + HexUint8ToString(m_dataLow) + ",X)";
}

std::string DisassemblerLineData::IndirectIndexedYToString() const
{
	return " ($" + HexUint8ToString(m_dataLow) + "),Y";
}


void Disassembler::Initialize(DisassemblerInitializeInfo& initializationInfo) {
	m_PRGLogicalBanks = initializationInfo.PRGLogicalBanks;
	m_PRGBankMapping = initializationInfo.PRGBankMapping;
	this->SetupPRGRom(initializationInfo.PRGROM);
	for (size_t i = 0; i < m_PRGROM.size(); i++) {
		m_disassembled.push_back(this->DisassembleBank(i, 0, 0));
	}
	m_currAddress = initializationInfo.address;
}

void Disassembler::Clear() {
	m_PRGROM.clear();
	m_PRGLogicalBanks.clear();
	m_PRGBankMapping.clear();
	m_disassembled.clear();
	m_currAddress = 0;
}

bool Disassembler::SetNextState(DisassemblerNextStateInfo& nextStateInfo) {
	m_currAddress = nextStateInfo.address;
	bool updateList = false;
	if (m_PRGBankMapping != nextStateInfo.PRGBankMapping) {
		m_PRGBankMapping = nextStateInfo.PRGBankMapping;

		// Banks have been switched, so update list
		updateList = true;
	}
	if (m_currAddress >= PRG_ROM_START_ADDRESS && m_currAddress <= PRG_ROM_END_ADDRESS) {
		size_t bankIndex = this->GetAddressPhysicalBankIndex(m_currAddress);
		uint16_t bankAddress = m_currAddress - PRG_ROM_START_ADDRESS - this->GetAddressLogicalBankIndex(m_currAddress) * m_PRGLogicalBanks[0].size;
		if (m_disassembled[bankIndex].find(bankAddress) == m_disassembled[bankIndex].end()) {
			// address not in disassembled bank - this means the disassembled code is not aligned with program code
			// Realign
			std::map<uint16_t, DisassemblerLineData> prefix = this->GetDisassembledDataUpToAddress(bankIndex, bankAddress);
			std::map<uint16_t, DisassemblerLineData> suffix = this->DisassembleBank(bankIndex, bankAddress, prefix.size());
			prefix.insert(suffix.begin(), suffix.end());
			m_disassembled[bankIndex] = prefix;

			// Need to update list to reflect changes
			updateList = true;
		}
	}
	return updateList;
}

size_t Disassembler::GetCurrentAddressIndex() const {
	size_t result = 0;
	for (size_t i = 0; i < m_PRGLogicalBanks.size(); i++) {
		if (m_currAddress >= m_PRGLogicalBanks[i].startAddress + m_PRGLogicalBanks[i].size) {
			result += m_disassembled[this->LogicalBankToPhysicalBankIndex(i)].size();
		}
		else {
			result += m_disassembled[this->LogicalBankToPhysicalBankIndex(i)].at(m_currAddress - m_PRGLogicalBanks[i].startAddress).GetListIndex();
			break;
		}
	}
	return result;
}

std::vector<std::string> Disassembler::GetProgramLines() {
	size_t bankSize = m_PRGLogicalBanks[0].size;
	std::vector<std::string> lines;
	for (const std::pair<unsigned int, size_t>& mapping : m_PRGBankMapping) {
		size_t bankIndex = this->LogicalBankToPhysicalBankIndex(mapping.first);
		for (const std::pair<uint16_t, DisassemblerLineData>& data : m_disassembled[bankIndex]) {
			lines.push_back(data.second.ToString(PRG_ROM_START_ADDRESS + (uint16_t)(mapping.first * bankSize)));
		}
	}
	return lines;
}

void Disassembler::SetupPRGRom(std::vector<uint8_t>& PRGROM) {
	// This assumes all banks are the same size
	size_t bankSize = m_PRGLogicalBanks[0].size;
	for (size_t i = 0; i < PRGROM.size(); i += bankSize) {
		m_PRGROM.push_back(std::vector<uint8_t>(PRGROM.begin() + i, PRGROM.begin() + i + bankSize));
	}
}

std::map<uint16_t, DisassemblerLineData> Disassembler::DisassembleBank(size_t physicalBankIndex, uint16_t startAddress, size_t startIndex) {
	std::queue<uint8_t> restOfProgramContent;
	for (size_t address = startAddress; address < m_PRGROM[physicalBankIndex].size(); address++) {
		restOfProgramContent.push(m_PRGROM[physicalBankIndex][address]);
	}
	uint16_t address = startAddress;
	uint16_t endAddress = m_PRGROM[physicalBankIndex].size();
	int currIndex = startIndex;
	std::map<uint16_t, DisassemblerLineData> result;

	DisassemblerLineData currLineData = DisassemblerLineData(startAddress, 0, INSTR_BRK, MODE_IMPLIED, 0, 0, currIndex);
	while (address <= endAddress && this->ConsumeNextLine(restOfProgramContent, address, currLineData, currIndex)) {
		result[address] = currLineData;
		address += currLineData.GetLength();
		currIndex++;
	}

	return result;
}

bool Disassembler::ConsumeNextLine(std::queue<uint8_t>& restOfProgramContent, uint16_t& currAddress, DisassemblerLineData& lineData, size_t currIndex) {
	if (restOfProgramContent.size() == 0) {
		return false;
	}

	uint8_t opCode = restOfProgramContent.front();
	restOfProgramContent.pop();
	CPUInstruction lineInstruction = CPU2A03::s_opCodeTable[opCode];
	int instructionLength = lineInstruction.GetInstructionLength();
	if (restOfProgramContent.size() < (size_t)instructionLength - 1) {
		return false;
	}

	uint8_t lowData = 0;
	uint8_t highData = 0;
	if (instructionLength > 1) {
		lowData = restOfProgramContent.front();
		restOfProgramContent.pop();
	}
	if (instructionLength > 2) {
		highData = restOfProgramContent.front();
		restOfProgramContent.pop();
	}

	lineData = DisassemblerLineData(currAddress, instructionLength, lineInstruction.mnemonic, lineInstruction.addressMode, lowData, highData, currIndex);
	return true;
}

size_t Disassembler::LogicalBankToPhysicalBankIndex(size_t logicalBankIndex) const {
	// This assumes all banks are the same size
	return m_PRGBankMapping.at(logicalBankIndex) / m_PRGLogicalBanks.at(logicalBankIndex).size;
}

size_t Disassembler::GetAddressLogicalBankIndex(uint16_t address) const {
	size_t bankIndex = 0;
	while (address >= m_PRGLogicalBanks[bankIndex].startAddress + m_PRGLogicalBanks[bankIndex].size) {
		bankIndex++;
	}
	return bankIndex;
}

size_t Disassembler::GetAddressPhysicalBankIndex(uint16_t address) const {
	// This assumes all banks are the same size
	size_t bankSize = m_PRGLogicalBanks[0].size;
	size_t logicalBankIndex = (address - PRG_ROM_START_ADDRESS) / bankSize;
	return m_PRGBankMapping.at(logicalBankIndex) / bankSize;
}

std::map<uint16_t, DisassemblerLineData> Disassembler::GetDisassembledDataUpToAddress(size_t physicalBankIndex, uint16_t targetAddress) {
	std::map<uint16_t, DisassemblerLineData> prefix;
	for (const std::pair<uint16_t, DisassemblerLineData>& data : m_disassembled[physicalBankIndex]) {
		if (data.first + data.second.GetLength() <= targetAddress) {
			prefix.insert(data);
		}
		else {
			break;
		}
	}
	return prefix;
}




wxDEFINE_EVENT(EVT_DISASSEMBLER_INITIALIZE, wxNESStateEvent<DisassemblerInitializeInfo>);
wxDEFINE_EVENT(EVT_DISASSEMBLER_NEXT_STATE, wxNESStateEvent<DisassemblerNextStateInfo>);

wxDisassemblerPanel::wxDisassemblerPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
	Bind(EVT_DISASSEMBLER_INITIALIZE, &wxDisassemblerPanel::OnInitialize, this);
	Bind(EVT_DISASSEMBLER_NEXT_STATE, &wxDisassemblerPanel::OnNextState, this);

	m_isInitialized = false;

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	m_programListBox = new wxLargeUnselectableListBox(this, wxID_ANY);

	sizer->Add(m_programListBox, 1, wxEXPAND);
	this->SetSizer(sizer);
	sizer->Fit(this);
}

bool wxDisassemblerPanel::IsInitialized() const {
	return m_isInitialized;
}

void wxDisassemblerPanel::OnInitialize(wxNESStateEvent<DisassemblerInitializeInfo>& evt) {
	DisassemblerInitializeInfo state = evt.GetState();
	m_disassembler.Initialize(state);
	this->UpdateList();
	m_isInitialized = true;
}

void wxDisassemblerPanel::Clear() {
	m_disassembler.Clear();
	m_programListBox->Clear();
	m_isInitialized = false;
}

void wxDisassemblerPanel::OnNextState(wxNESStateEvent<DisassemblerNextStateInfo>& evt) {
	if (evt.GetState().address >= 0x8000 && evt.GetState().address <= 0xFFFF) {
		DisassemblerNextStateInfo state = evt.GetState();
		if (m_disassembler.SetNextState(state)) {
			this->UpdateList();
		}
		m_programListBox->SelectItem(m_disassembler.GetCurrentAddressIndex());
	}
}

void wxDisassemblerPanel::UpdateList() {
	wxArrayString lines;
	m_programListBox->Clear();
	for (auto const& line : m_disassembler.GetProgramLines()) {
		lines.push_back(wxString(line));
	}
	m_programListBox->SetItems(lines);
}