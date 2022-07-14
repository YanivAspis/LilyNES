#include "wxDisassemblerPanel.h"
#include "../../BitwiseUtils.h"
#include <sstream>
#include <cstdint>
#include <iomanip>

using namespace BitwiseUtils;

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

std::map<AddressingMode, int> Disassembler::s_addressingModeToLength = {
	{MODE_IMPLIED, 1},
	{MODE_ACCUMULATOR, 1},
	{MODE_IMMEDIATE, 2},
	{MODE_ZERO_PAGE, 2},
	{MODE_ABSOLUTE, 3},
	{MODE_RELATIVE, 2},
	{MODE_INDIRECT, 3},
	{MODE_ZERO_PAGE_INDEXED_X, 2},
	{MODE_ZERO_PAGE_INDEXED_Y, 2},
	{MODE_ABSOLUTE_INDEXED_X, 3},
	{MODE_ABSOLUTE_INDEXED_Y, 3},
	{MODE_INDEXED_INDIRECT_X, 2},
	{MODE_INDIRECT_INDEXED_Y, 2}
};


std::string DisassemblerLineData::ToString() {
	std::stringstream result;
	result << "0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << (unsigned int)m_beginAddress << ": ";
	result << Disassembler::s_mnemonicToString[m_mnemonic];

	switch (m_addressingMode) {
		case MODE_IMPLIED:
			result << this->ImpliedToString();
			break;
		case MODE_ACCUMULATOR:
			result << this->AccumulatorToString();
			break;
		case MODE_IMMEDIATE:
			result << this->ImmediateToString();
			break;
		case MODE_ZERO_PAGE:
			result << this->ZeroPageToString();
			break;
		case MODE_ABSOLUTE:
			result << this->AbsoluteToString();
			break;
		case MODE_RELATIVE:
			result << this->RelativeToString();
			break;
		case MODE_INDIRECT:
			result << this->IndirectToString();
			break;
		case MODE_ZERO_PAGE_INDEXED_X:
			result << this->ZeroPageIndexedXToString();
			break;
		case MODE_ZERO_PAGE_INDEXED_Y:
			result << this->ZeroPageIndexedYToString();
			break;
		case MODE_ABSOLUTE_INDEXED_X:
			result << this->AbsoluteIndexedXToString();
			break;
		case MODE_ABSOLUTE_INDEXED_Y:
			result << this->AbsoluteIndexedYToString();
			break;
		case MODE_INDEXED_INDIRECT_X:
			result << this->IndexedIndirectXToString();
			break;
		case MODE_INDIRECT_INDEXED_Y:
			result << this->IndirectIndexedYToString();
			break;
	}

	return result.str();
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

std::string DisassemblerLineData::ImpliedToString()
{
	return "";
}

std::string DisassemblerLineData::AccumulatorToString()
{
	return "";
}

std::string DisassemblerLineData::ImmediateToString()
{
	std::stringstream result;
	result << " #$" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)m_dataLow;
	return result.str();
}

std::string DisassemblerLineData::ZeroPageToString()
{
	std::stringstream result;
	result << " $" << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)m_dataLow;
	return result.str();
}

std::string DisassemblerLineData::AbsoluteToString()
{
	std::stringstream result;
	result << " $" << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)m_dataHigh << std::setw(2) << (unsigned int)m_dataLow;
	return result.str();
}

std::string DisassemblerLineData::RelativeToString()
{
	std::stringstream result;
	uint16_t resultAddress = Add16Bit(m_beginAddress, m_length);
	// handle signed values and convert to 16-bit
	uint16_t unsigned_relative_address = 0;
	if (TestBit8(m_dataLow, 7)) {
		unsigned_relative_address = CombineBytes(m_dataLow, 0xFF);
	}
	else {
		unsigned_relative_address = m_dataLow;
	}
	resultAddress = Add16Bit(resultAddress, unsigned_relative_address);

	result << " $" << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)m_dataLow << " [$" << std::setw(4) << (unsigned int)resultAddress << "]";
	return result.str();
}

std::string DisassemblerLineData::IndirectToString()
{
	std::stringstream result;
	result << " ($" << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)m_dataHigh << std::setw(2) << (unsigned int)m_dataLow << ")";
	return result.str();
}

std::string DisassemblerLineData::ZeroPageIndexedXToString()
{
	std::stringstream result;
	result << " $" << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)m_dataLow << ",X";
	return result.str();
}

std::string DisassemblerLineData::ZeroPageIndexedYToString()
{
	std::stringstream result;
	result << " $" << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)m_dataLow << ",Y";
	return result.str();
}

std::string DisassemblerLineData::AbsoluteIndexedXToString()
{
	std::stringstream result;
	result << " $" << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)m_dataHigh << std::setw(2) << (unsigned int)m_dataLow << ",X";
	return result.str();
}

std::string DisassemblerLineData::AbsoluteIndexedYToString()
{
	std::stringstream result;
	result << " $" << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)m_dataHigh << std::setw(2) << (unsigned int)m_dataLow << ",Y";
	return result.str();
}

std::string DisassemblerLineData::IndexedIndirectXToString()
{
	std::stringstream result;
	result << " ($" << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)m_dataLow << ",X)";
	return result.str();
}

std::string DisassemblerLineData::IndirectIndexedYToString()
{
	std::stringstream result;
	result << " ($" << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)m_dataLow << "),Y";
	return result.str();
}


void Disassembler::Initialize(std::vector<uint8_t> programContent, uint16_t beginAddress, uint16_t endAddress, uint16_t currAddress) {
	std::queue<uint8_t> restOfProgramContent;
	for (size_t address = beginAddress; address < beginAddress + programContent.size(); address++) {
		restOfProgramContent.push(programContent[address - 0x8000]);
	}
	uint16_t address = beginAddress;
	uint16_t lastAddress = beginAddress;
	int currIndex = 0;
	DisassemblerLineData currLineData = DisassemblerLineData(beginAddress, 0, INSTR_BRK, MODE_IMPLIED, 0, 0);
	while (address <= endAddress && this->ConsumeNextLine(restOfProgramContent, address, currLineData)) {
		m_programData.push_back(currLineData);
		m_addressToIndex[lastAddress] = currIndex;
		lastAddress = address;
		currIndex++;
	}
	if (programContent.size() <= 16 * 1024) {
		std::vector<DisassemblerLineData> programMirror;
		for (DisassemblerLineData lineData : m_programData) {
			programMirror.push_back(DisassemblerLineData(lineData.GetAddress() + 0x4000, lineData.GetLength(), lineData.GetMnemonic(), 
				lineData.GetAddressingMode(), lineData.GetDataLow(), lineData.GetDataHigh()));
		}
		for (DisassemblerLineData lineData : programMirror) {
			m_programData.push_back(lineData);
			m_addressToIndex[lineData.GetAddress()] = currIndex;
			currIndex++;
		}
	}
	m_currAddress = currAddress;
}

void Disassembler::Clear() {
	m_programData.clear();
	m_addressToIndex.clear();
	m_currAddress = 0;
}

void Disassembler::SetNextAddress(uint16_t newAddress) {
	m_currAddress = newAddress;
}

bool Disassembler::ConsumeNextLine(std::queue<uint8_t>& restOfProgramContent, uint16_t& currAddress, DisassemblerLineData& lineData) {
	if (restOfProgramContent.size() == 0) {
		return false;
	}

	uint8_t opCode = restOfProgramContent.front();
	restOfProgramContent.pop();
	CPUInstruction lineInstruction = CPU2A03::s_opCodeTable[opCode];
	int instructionLength = s_addressingModeToLength[lineInstruction.addressMode];
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

	uint16_t beginAddress = currAddress;
	lineData = DisassemblerLineData(beginAddress, instructionLength, lineInstruction.mnemonic, lineInstruction.addressMode, lowData, highData);
	currAddress += instructionLength;
	return true;
}

int Disassembler::GetCurrentAddressIndex() const {
	return m_addressToIndex.at(m_currAddress);
}

std::vector<std::string> Disassembler::GetProgramLines() {
	std::vector<std::string> lines;
	for (auto & lineData : m_programData) {
		lines.push_back(lineData.ToString());
	}
	return lines;
}


wxDEFINE_EVENT(EVT_DISASSEMBLER_INITIALIZE, wxNESStateEvent<DisassemblerInitializeInfo>);
wxDEFINE_EVENT(EVT_DISASSEMBLER_NEXT_ADDRESS, wxNESStateEvent<uint16_t>);

wxDisassemblerPanel::wxDisassemblerPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
	Bind(EVT_DISASSEMBLER_INITIALIZE, &wxDisassemblerPanel::OnInitialize, this);
	Bind(EVT_DISASSEMBLER_NEXT_ADDRESS, &wxDisassemblerPanel::OnNextAddress, this);

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
	m_disassembler.Initialize(evt.GetState().programContent, evt.GetState().beginAddress, evt.GetState().endAddress, evt.GetState().currAddress);
	wxArrayString lines;
	for (auto const& line : m_disassembler.GetProgramLines()) {
		lines.push_back(wxString(line));
	}
	m_programListBox->SetItems(lines);
	m_isInitialized = true;
}

void wxDisassemblerPanel::Clear() {
	m_disassembler.Clear();
	m_programListBox->Clear();
	m_isInitialized = false;
}

void wxDisassemblerPanel::OnNextAddress(wxNESStateEvent<uint16_t>& evt) {
	if (evt.GetState() >= 0x8000 && evt.GetState() <= 0xFFFF) {
		m_disassembler.SetNextAddress(evt.GetState());
		m_programListBox->SelectItem(m_disassembler.GetCurrentAddressIndex());
	}
}