#include <sstream>
#include <iomanip>
#include <fstream>

#include <boost/filesystem.hpp>


#include "NESUtils.h"
#include "../nes/NES.h"
//#include "../nes/CPU/CPU2A03.h"

namespace NESUtils {
	std::string HexUint8ToString(uint8_t num) {
		std::stringstream result;
		result << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << (unsigned int)num;
		return result.str();
	}

	std::string DecUint8ToString(uint8_t num) {
		std::stringstream result;
		result << std::setfill('0') << std::setw(3) << (unsigned int)num;
		return result.str();
	}

	std::string HexUint16ToString(uint16_t num) {
		std::stringstream result;
		result << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << (unsigned int)num;
		return result.str();
	}

	std::string IntToString(int num, unsigned int minWidth) {
		std::stringstream result;
		result << std::setfill('0') << std::setw(minWidth) << num;
		return result.str();
	}


	// The code below is for handling the nestest.nes ROM for testing the CPU implementation

	NESTestInstructionLine::NESTestInstructionLine() {
		this->SetRegisters(0, 0, 0, 0, 0, 0, 0);
		this->SetCycles(0, 0, 0);
	}

	void NESTestInstructionLine::SetRegisters(uint16_t PC, uint8_t A, uint8_t X, uint8_t Y, uint8_t P, uint8_t SP, uint8_t ram0002value) {
		m_PC = PC;
		m_A = A;
		m_X = X;
		m_Y = Y;
		m_P = P;
		m_SP = SP;
		m_ram0002value = ram0002value;
	}

	void NESTestInstructionLine::SetCycles(unsigned int scanline, unsigned int dot, unsigned int numCPUCycles) {
		m_scanline = scanline;
		m_dot = dot;
		m_numCPUCycles = numCPUCycles;
	}

	void NESTestInstructionLine::SetInstructionBytes(std::vector<uint8_t> instructionBytes) {
		m_instructionBytes = instructionBytes;
	}

	std::string NESTestInstructionLine::ToString() const {
		std::string result = HexUint16ToString(m_PC) + "\t";
		for (uint8_t byte : m_instructionBytes) {
			result += HexUint8ToString(byte) + " ";
		}
		result += "\tA: " + HexUint8ToString(m_A);
		result += "  X: " + HexUint8ToString(m_X);
		result += "  Y: " + HexUint8ToString(m_Y);
		result += "  P: " + HexUint8ToString(m_P);
		result += "  SP: " + HexUint8ToString(m_SP);
		result += "  Addr2: " + HexUint8ToString(m_ram0002value);
		result += " (" + std::to_string(m_scanline) + "," + std::to_string(m_dot) + ")";
		result += "  Cycles: " + std::to_string(m_numCPUCycles);

		return result;
	}

	void RunCPUTest(std::string romPath, std::string outputPath) {
		INESFile* romFile = new INESFile(romPath);
		NES* nes = new NES();
		nes->LoadROM(*romFile);
		nes->HardReset();

		NESState state = nes->GetState();
		state.cpuState.regPC = 0xC000;
		nes->LoadState(state);

		std::list<NESTestInstructionLine> lines;
		NESTestInstructionLine currInstruction;

		try {
			while (true) {
				nes->RunUntilNextInstruction();
				state = nes->GetState();
				currInstruction.SetRegisters(state.cpuState.regPC, state.cpuState.regA, state.cpuState.regX, state.cpuState.regY, state.cpuState.regP.value, state.cpuState.regS, state.ramState.content[0x0002]);
				currInstruction.SetCycles(state.ppuState.scanline, state.ppuState.dot, state.cpuState.cycleCount);
				currInstruction.SetInstructionBytes(nes->ProbeCurrentCPUInstruction());
				lines.push_back(currInstruction);
			}
		}
		catch (IllegalInstructionException ex) {}

		std::ofstream outputFile;
		outputFile.open(outputPath, std::ios::out | std::ios::trunc);
		for (const NESTestInstructionLine& line : lines) {
			outputFile << line.ToString() << "\n";
		}
		outputFile.close();

		delete romFile;
		delete nes;
	}

	HighPassFilter::HighPassFilter(float cutoffFrequency, float sampleRate) {
		float RC = 1.0 / (2 * M_PI * cutoffFrequency);
		float dt = 1.0 / sampleRate;
		m_alpha = RC / (RC + dt);
		m_firstSampleReceived = false;
		m_lastOriginalSample = 0;
		m_lastResampled = 0;
	}

	void HighPassFilter::Restart() {
		m_firstSampleReceived = false;
		m_lastOriginalSample = 0;
		m_lastResampled = 0;
	}

	float HighPassFilter::Filter(float sample) {
		if (m_firstSampleReceived) {
			m_lastResampled = m_alpha * (m_lastResampled + sample - m_lastOriginalSample);
			m_lastOriginalSample = sample;
		}
		else {
			m_lastResampled = sample;
			m_lastOriginalSample = sample;
			m_firstSampleReceived = true;
		}
		return m_lastResampled;
	}

	LowPassFilter::LowPassFilter(float cutoffFrequency, float sampleRate) {
		float RC = 1.0 / (2 * M_PI * cutoffFrequency);
		float dt = 1.0 / sampleRate;
		m_alpha = dt / (RC + dt);
		m_lastSample = 0;
	}

	void LowPassFilter::Restart() {
		m_lastSample = 0;
	}

	float LowPassFilter::Filter(float sample) {
		m_lastSample += m_alpha * (sample - m_lastSample);
		return m_lastSample;
	}

	namespace MD5 {
		std::array<uint8_t, CHECKSUM_SIZE> ComputeChecksum(const std::vector<uint8_t>& message) {
			std::vector<uint8_t> paddedMessage = PadMessage(message);
			uint32_t a0 = INIT_A0;
			uint32_t b0 = INIT_B0;
			uint32_t c0 = INIT_C0;
			uint32_t d0 = INIT_D0;
			for (unsigned int chunkIndex = 0; chunkIndex < paddedMessage.size() / CHUNK_SIZE; chunkIndex++) {
				size_t beginIndex = chunkIndex * CHUNK_SIZE;
				size_t endIndex = beginIndex + CHUNK_SIZE;
				std::array<uint8_t, CHUNK_SIZE> chunk;
				std::copy(paddedMessage.begin() + beginIndex, paddedMessage.begin() + endIndex, chunk.begin());
				ComputeChunkHash(chunk, a0, b0, c0, d0);
			}

			std::array<uint8_t, CHECKSUM_SIZE> result;

			std::array<uint8_t, 4> split = SplitBytes32(a0);
			for (unsigned int i = 0; i < 4; i++) {
				result[i] = split[i];
			}

			split = SplitBytes32(b0);
			for (unsigned int i = 0; i < 4; i++) {
				result[4 + i] = split[i];
			}

			split = SplitBytes32(c0);
			for (unsigned int i = 0; i < 4; i++) {
				result[8 + i] = split[i];
			}

			split = SplitBytes32(d0);
			for (unsigned int i = 0; i < 4; i++) {
				result[12 + i] = split[i];
			}

			return result;
		}

		std::string ChecksumToString(const std::array<uint8_t, CHECKSUM_SIZE>& checksum) {
			std::string resultStr(32, '0');
			for (unsigned int i = 0; i < 16; i++) {
				std::string byteStr = HexUint8ToString(checksum[i]);
				std::copy(byteStr.begin() + 2, byteStr.end(), resultStr.begin() + 2 * i);
			}
			return resultStr;
		}

		std::vector<uint8_t> PadMessage(const std::vector<uint8_t>& message) {
			uint64_t messageLength = message.size() * 8;
			size_t extraPadding = CHUNK_SIZE - (message.size() + MESSAGE_LENGTH_BYTES + 1) % CHUNK_SIZE;
			if (extraPadding == CHUNK_SIZE) {
				extraPadding = 0;
			}
			std::vector<uint8_t> paddedMessage = message;
			paddedMessage.push_back(0x80);
			while (extraPadding > 0) {
				paddedMessage.push_back(0);
				extraPadding--;
			}

			for (uint8_t& num : SplitBytes32(messageLength & 0xFFFFFFFF)) {
				paddedMessage.push_back(num);
			}
			for (uint8_t& num : SplitBytes32((messageLength & 0xFFFFFFFF00000000) >> 32)) {
				paddedMessage.push_back(num);
			}

			return paddedMessage;
		}

		void ComputeChunkHash(std::array<uint8_t, CHUNK_SIZE>& chunk, uint32_t& a0, uint32_t& b0, uint32_t& c0, uint32_t& d0) {
			std::array<uint32_t, CHUNK_NUM_DWORDS> DWORDArray = GetDWORDArray(chunk);

			uint32_t A = a0;
			uint32_t B = b0;
			uint32_t C = c0;
			uint32_t D = d0;
			uint32_t F = 0;
			size_t g = 0;

			for (unsigned int round = 0; round < MD5_ROUNDS; round++) {
				ComputeFgValues(round, A, B, C, D, F, g);
				F = Add32Bit(F, Add32Bit(A, Add32Bit(CONSTANTS[round], DWORDArray[g])));
				A = D;
				D = C;
				C = B;
				B = Add32Bit(B, RotateLeft32Bit(F, SHIFT_AMOUNTS[round]));
			}

			a0 = Add32Bit(a0, A);
			b0 = Add32Bit(b0, B);
			c0 = Add32Bit(c0, C);
			d0 = Add32Bit(d0, D);

		}

		std::array<uint32_t, CHUNK_NUM_DWORDS> GetDWORDArray(std::array<uint8_t, CHUNK_SIZE>& chunk) {
			std::array<uint32_t, CHUNK_NUM_DWORDS> DWORDArray;
			for (unsigned int DWORDIndex = 0; DWORDIndex < CHUNK_NUM_DWORDS; DWORDIndex++) {
				DWORDArray[DWORDIndex] = CombineBytes32(chunk[DWORDIndex * 4], chunk[DWORDIndex * 4 + 1], chunk[DWORDIndex * 4 + 2], chunk[DWORDIndex * 4 + 3]);
			}

			return DWORDArray;
		}

		void ComputeFgValues(unsigned int round, uint32_t A, uint32_t B, uint32_t C, uint32_t D, uint32_t& F, size_t& g) {
			if (round >= 0 && round < 16) {
				F = (B & C) | (~B & D);
				g = round;
			}
			else if (round >= 16 && round < 32) {
				F = (D & B) | (~D & C);
				g = (5*round + 1) % 16;
			}
			else if (round >= 32 && round < 48) {
				F = B ^ C ^ D;
				g = (3*round + 5) % 16;
			}
			else if (round >= 48 && round < 64) {
				F = C ^ (B | ~D);
				g = (7 * round) % 16;
			}
		}
	}

	void SaveBatteryBackedRAM(std::array<uint8_t, MD5::CHECKSUM_SIZE> checksum, std::vector<uint8_t> PRGRAMContent) {
		boost::filesystem::create_directories("save/battery/");
		std::string savePath = "save/battery/" + MD5::ChecksumToString(checksum) + ".sav";
		std::ofstream saveFile(savePath, std::ios::binary);
		std::ostream_iterator<char> saveFileIterator(saveFile);
		std::copy(PRGRAMContent.begin(), PRGRAMContent.end(), saveFileIterator);
	}

	bool LoadBatteryBackedRAM(std::array<uint8_t, MD5::CHECKSUM_SIZE> checksum, std::vector<uint8_t>& PRGRAMContent) {
		std::string loadPath = "save/battery/" + MD5::ChecksumToString(checksum) + ".sav";
		std::ifstream loadFile(loadPath, std::ios::binary);
		if (loadFile.fail()) {
			return false;
		}

		std::istreambuf_iterator<char> loadFileIterator(loadFile);
		PRGRAMContent = std::vector<uint8_t>(loadFileIterator, {});

		return true;
	}

	void SaveStateToFile(std::array<uint8_t, MD5::CHECKSUM_SIZE> checksum, std::string filepath, NESState& state) {
		// TODO: What if we cannot save to file
		std::ofstream outfile(filepath, std::fstream::binary | std::fstream::out);
		boost::archive::binary_oarchive outarchive(outfile);
		outarchive << boost::serialization::make_array(checksum.data(), checksum.size());
		outarchive << state;
	}

	void QuickSaveStateToFile(std::array<uint8_t, MD5::CHECKSUM_SIZE> checksum, unsigned int slot, NESState& state) {
		std::string saveDirectory = "save/state/" + MD5::ChecksumToString(checksum);
		boost::filesystem::create_directories(saveDirectory);
		SaveStateToFile(checksum, saveDirectory + "/" + std::to_string(slot) + ".sav", state);
	}

	NESState LoadStateFromFile(std::array<uint8_t, MD5::CHECKSUM_SIZE> checksum, std::string filepath) {
		// TODO: What if file cannot be read
		std::ifstream infile(filepath, std::fstream::binary | std::fstream::in);
		boost::archive::binary_iarchive inarchive(infile);
		std::array<uint8_t, MD5::CHECKSUM_SIZE> checksumFromFile;
		inarchive >> boost::serialization::make_array(checksumFromFile.data(), checksumFromFile.size());
		if (checksumFromFile != checksum) {
			// TODO: ROM mismatch
		}
		NESState state;
		inarchive >> state;
		return state;
	}

	NESState QuickLoadStateFromFile(std::array<uint8_t, MD5::CHECKSUM_SIZE> checksum, unsigned int slot) {
		std::string loadPath = "save/state/" + MD5::ChecksumToString(checksum) + "/" + std::to_string(slot) + ".sav";
		return LoadStateFromFile(checksum, loadPath);
	}

	std::vector<unsigned int> GetSaveStateSlotsAvailable(std::array<uint8_t, MD5::CHECKSUM_SIZE> checksum) {
		std::vector<unsigned int> slots;
		std::string saveStatePath = "save/state/" + MD5::ChecksumToString(checksum) + "/";
		if (! boost::filesystem::exists(saveStatePath)) {
			return slots;
		}

		// Find all sav files
		boost::filesystem::directory_iterator endIt;
		for (boost::filesystem::directory_iterator dirIt(saveStatePath); dirIt != endIt; dirIt++) {
			if (boost::filesystem::is_regular_file(dirIt->status())) {
				if (boost::filesystem::extension(dirIt->path().filename()) == ".sav") {
					std::stringstream filename;
					filename << dirIt->path().stem();
					slots.push_back(std::atoi(filename.str().c_str()));
				}
			}
		}
		return slots;
	}
}
