#ifndef FW102_h	//check for multiple inclusions
#define FW102_h

#include "Arduino.h"

#define ReplyBufferSize 16

typedef void ( *FinishedListener )();

class FW102
{
	public:
		enum class CommandsType : uint8_t {Position, PositionCount, TriggerMode, SpeedMode, SensorMode, BaudMode, Count, Save, Error};
		enum class CommandValueType : uint8_t {None, Integer, Bool, Count};
		enum class TransmissionType : uint8_t {Get, Set, Error, None, Count};
		enum class ErrorType : uint8_t {CommandNotDefined, ArguementInvalid, Count};
		enum class ReplyValueType : uint8_t {None, Integer, Bool, Error, Count};
		enum class RecievingPart : uint8_t {Echo, Value, Count};
		union CommandValue
		{
			uint8_t IntegerValue;
			bool BoolValue;
		};
		union ReplyValue
		{
			uint8_t IntegerValue;
			bool BoolValue;
			ErrorType ErrorValue;
		};
		struct CommandStringList
		{
			CommandsType Command;
			CommandValueType ValueType;
			const char* String;
			uint8_t Count;
		};
		struct Command
		{
			CommandsType Command;
			TransmissionType Type;
			CommandValue Value;
			CommandValueType ValueType;
		};
		struct Reply
		{
			CommandsType Command;
			TransmissionType Type;
			ReplyValue Value;
			ReplyValueType ValueType;
		};
		struct ReplyReturn
		{
			ReplyValue Value;
			ReplyValueType ValueType;
		};
		FW102(HardwareSerial* serial); //Invoke with FW102(&SerialN);
		bool SendSetPosition(uint8_t Position);
		bool SendGetPosition();
		bool GetPositionCount();
		bool SetTriggerMode(bool TriggerMode);
		bool GetTriggerMode();
		bool SetSpeedMode(bool SpeedMode);
		bool GetSpeedMode();
		bool SetSensorMode(bool SensorMode);
		bool GetSensorMode();
		bool SetBaudRate(bool BaudMode);
		bool GetBaudRate();
		bool SaveSettings();
		bool SetRecievedCallback(FinishedListener Finished);
		bool IsBusy();
		void CheckSerial();
	private:
		HardwareSerial* _HardwareSerial;
		FinishedListener RecievedCallback;
		Reply CurrentReply;
		RecievingPart CurrentRecievingPart;
		Command CurrentCommand;
		uint8_t PositionCount;
		uint32_t TransmitTime;
		bool Busy;
		bool ExpectReply;
		char* ReplyBuffer;
		uint8_t ReplyBufferIndex;
		uint8_t CommandRetries;
		bool SendCurrentCommand();
		char UInt8ToChar(const uint8_t Input);
		uint8_t CharToUInt8(const char Character);
		void ParseEcho(char Character);
		void ParseValue(char Character);
		bool SendSetBoolCommand(bool Setting, CommandsType SelectedType);
		bool SendGetBoolCommand(CommandsType SelectedType);
		void CheckReply();
		uint8_t GetReplyValue();
		static const CommandStringList CommandLibrary[];
		static const uint8_t MaxCommandRetries;
		static const uint32_t TimeOut;
		static const char CarriageReturnCharacter;
		static const char EndOfLineCharacter;
		static const char SpaceCharacter;
		static const char PromptCharacter;
		static const char SetCharacter;
		static const char GetCharacter;
		static const char TrueCharacter;
		static const char FalseCharacter;
};
#endif