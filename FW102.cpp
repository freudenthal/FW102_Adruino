#include "FW102.h"

const char FW102::CarriageReturnCharacter = '\r';
const char FW102::EndOfLineCharacter = '\n';
const char FW102::SpaceCharacter = ' ';
const char FW102::PromptCharacter = '>';
const char FW102::SetCharacter = '=';
const char FW102::GetCharacter = '?';
const char FW102::TrueCharacter = '1';
const char FW102::FalseCharacter = '0';
const uint32_t FW102::TimeOut = 5000000;
const uint8_t FW102::MaxCommandRetries = 8;
const FW102::CommandStringList FW102::CommandLibrary[] =
{
	{CommandsType::Position,CommandValueType::Integer,"pos",3},
	{CommandsType::PositionCount,CommandValueType::Integer,"pcount",6},
	{CommandsType::TriggerMode,CommandValueType::Bool,"trig",4},
	{CommandsType::SpeedMode,CommandValueType::Bool,"speed",5},
	{CommandsType::SensorMode,CommandValueType::Bool,"sensor",6},
	{CommandsType::BaudMode,CommandValueType::Bool,"baud",4},
	{CommandsType::Save,CommandValueType::None,"save",4}
};
FW102::FW102(HardwareSerial *serial)
{
	_HardwareSerial = serial;
	PositionCount = 6;
	CurrentRecievingPart = RecievingPart::Echo;
	RecievedCallback = 0;
	Busy = false;
	ExpectReply = false;
	ReplyBuffer = new char[ReplyBufferSize]();
	ReplyBufferIndex = 0;
	CommandRetries = 0;
}
bool FW102::SendSetPosition(uint8_t Position)
{
	if (!Busy)
	{
		Busy = true;
		CommandRetries = 0;
		if ( (Position < 1)||(Position > PositionCount) )
		{
			return false;
		}
		CurrentCommand.Command = CommandLibrary[static_cast<uint8_t>(CommandsType::Position)].Command;
		CurrentCommand.Type = TransmissionType::Set;
		CurrentCommand.Value.IntegerValue = Position;
		CurrentCommand.ValueType = CommandValueType::Integer;
		return SendCurrentCommand();
	}
	else
	{
		return false;
	}
}
bool FW102::SendGetPosition()
{
	if (!Busy)
	{
		Busy = true;
		CommandRetries = 0;
		CurrentCommand.Command = CommandLibrary[static_cast<uint8_t>(CommandsType::Position)].Command;
		CurrentCommand.Type = TransmissionType::Get;
		CurrentCommand.ValueType = CommandValueType::Integer;
		return SendCurrentCommand();
	}
	else
	{
		return false;
	}
}
bool FW102::GetPositionCount()
{
	if (!Busy)
	{
		Busy = true;
		CommandRetries = 0;
		CurrentCommand.Command = CommandLibrary[static_cast<uint8_t>(CommandsType::PositionCount)].Command;
		CurrentCommand.Type = TransmissionType::Get;
		CurrentCommand.ValueType = CommandValueType::Integer;
		return SendCurrentCommand();
	}
	else
	{
		return false;
	}
}
bool FW102::SendSetBoolCommand(bool Setting, CommandsType SelectedType)
{
	if (!Busy)
	{
		Busy = true;
		CommandRetries = 0;
		CurrentCommand.Command = CommandLibrary[static_cast<uint8_t>(SelectedType)].Command;
		CurrentCommand.Type = TransmissionType::Get;
		CurrentCommand.ValueType = CommandValueType::Bool;
		CurrentCommand.Value.BoolValue = Setting;
		return SendCurrentCommand();
	}
	else
	{
		return false;
	}
}
bool FW102::SendGetBoolCommand(CommandsType SelectedType)
{
	if (!Busy)
	{
		Busy = true;
		CommandRetries = 0;
		CurrentCommand.Command = CommandLibrary[static_cast<uint8_t>(SelectedType)].Command;
		CurrentCommand.Type = TransmissionType::Get;
		CurrentCommand.ValueType = CommandValueType::Bool;
		return SendCurrentCommand();
	}
	else
	{
		return false;
	}
}
bool FW102::SetTriggerMode(bool TriggerMode)
{
	return SendSetBoolCommand(TriggerMode, CommandsType::TriggerMode);
}
bool FW102::GetTriggerMode()
{
	return SendGetBoolCommand(CommandsType::TriggerMode);
}
bool FW102::SetSpeedMode(bool SpeedMode)
{
	return SendSetBoolCommand(SpeedMode, CommandsType::SpeedMode);
}
bool FW102::GetSpeedMode()
{
	return SendGetBoolCommand(CommandsType::SpeedMode);
}
bool FW102::SetSensorMode(bool SensorMode)
{
	return SendSetBoolCommand(SensorMode, CommandsType::SensorMode);
}
bool FW102::GetSensorMode()
{
	return SendGetBoolCommand(CommandsType::SensorMode);
}
bool FW102::SetBaudRate(bool BaudMode)
{
	return SendSetBoolCommand(BaudMode, CommandsType::BaudMode);
}
bool FW102::GetBaudRate()
{
	return SendGetBoolCommand(CommandsType::BaudMode);
}
bool FW102::SaveSettings()
{
	if (!Busy)
	{
		Busy = true;
		CurrentCommand.Command = CommandLibrary[static_cast<uint8_t>(CommandsType::Save)].Command;
		CurrentCommand.Type = TransmissionType::None;
		CurrentCommand.ValueType = CommandValueType::None;
		CurrentCommand.Value.BoolValue = false;
		return SendCurrentCommand();
	}
	else
	{
		return false;
	}
}
bool FW102::SetRecievedCallback(FinishedListener Finished)
{
	if (!Finished)
	{
		return false;
	}
	RecievedCallback = Finished;
	return true;
}
bool FW102::SendCurrentCommand()
{
	const uint8_t CommandIndex = static_cast<uint8_t>(CurrentCommand.Command);
	const uint8_t* StringToSend = reinterpret_cast<const uint8_t*>(CommandLibrary[CommandIndex].String);
	const uint8_t CharsToSend = CommandLibrary[CommandIndex].Count;
	_HardwareSerial->write(StringToSend, CharsToSend);
	//Serial.print(String((char*)StringToSend));
	if (CurrentCommand.Type == TransmissionType::Set)
	{
		_HardwareSerial->write(SetCharacter);
		//Serial.print(SetCharacter);
		if (CurrentCommand.ValueType == CommandValueType::Integer)
		{
			_HardwareSerial->write(UInt8ToChar(CurrentCommand.Value.IntegerValue));
			//Serial.print(CurrentCommand.Value.IntegerValue);
		}
		else if (CurrentCommand.ValueType == CommandValueType::Bool)
		{
			if (CurrentCommand.Value.BoolValue)
			{
				_HardwareSerial->write(TrueCharacter);
				//Serial.print(TrueCharacter);
			}
			else
			{
				_HardwareSerial->write(FalseCharacter);
				//Serial.print(FalseCharacter);
			}
		}
		else
		{
			return false;
		}
	}
	else if (CurrentCommand.Type == TransmissionType::Get)
	{
		_HardwareSerial->write(GetCharacter);
		//Serial.print(GetCharacter);
	}
	else if (CurrentCommand.Type == TransmissionType::None)
	{

	}
	else
	{
		return false;
	}
	_HardwareSerial->write(CarriageReturnCharacter);
	_HardwareSerial->flush();
	//Serial.print(CarriageReturnCharacter);
	TransmitTime = micros();
	ReplyBufferIndex = 0;
	CurrentRecievingPart = RecievingPart::Echo;
	CurrentReply.ValueType = ReplyValueType::None;
	ExpectReply = true;
	return true;
}
char FW102::UInt8ToChar(const uint8_t Input)
{
	switch (Input)
	{
		case 0:
			return '0';
		case 1:
			return '1';
		case 2:
			return '2';
		case 3:
			return '3';
		case 4:
			return '4';
		case 5:
			return '5';
		case 6:
			return '6';
		case 7:
			return '7';
		case 8:
			return '8';
		case 9:
			return '9';
		default:
			return '*';
	}
}
uint8_t CharToUint8t(const char Input)
{
	switch (Input)
	{
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
		default:
			return 10;
	}
}
void FW102::CheckSerial()
{
	if (ExpectReply)
	{
		if(_HardwareSerial->available() > 0)
		{
			char Character = (char)_HardwareSerial->read();
			switch (CurrentRecievingPart)
			{
				case RecievingPart::Echo:
					ParseEcho(Character);
					break;
				case RecievingPart::Value:
					ParseValue(Character);
					break;
				default:
					break;
			}
		}
		else if ( (micros() - TransmitTime) > TimeOut )
		{
			Serial.println("Timeout on filter.");
			CurrentReply.Command = CommandsType::Error;
			CurrentReply.Type = TransmissionType::Error;
			CurrentReply.ValueType = ReplyValueType::Error;
			ReplyBufferIndex = 0;
			CurrentRecievingPart = RecievingPart::Echo;

			CheckReply();
		}
	}
	else
	{
		if(_HardwareSerial->available() > 0)
		{
			_HardwareSerial->clear();
		}
	}
}
void FW102::ParseEcho(char Character)
{
	//Serial.print(Character);
	if (isalpha(Character))
	{
		ReplyBuffer[ReplyBufferIndex] = Character;
		if (ReplyBufferIndex < (ReplyBufferSize-2) )
		{
			ReplyBufferIndex++;
		}
	}
	else if ( (Character == SetCharacter) || (Character == GetCharacter) || (Character == SpaceCharacter) )
	{
		CurrentRecievingPart = RecievingPart::Value;
		if ( Character == SetCharacter)
		{
			CurrentReply.Type = TransmissionType::Set;
		}
		else if ( Character == GetCharacter)
		{
			CurrentReply.Type = TransmissionType::Get;
		}
		else
		{
			CurrentReply.Type = TransmissionType::Error;
			CurrentReply.ValueType = ReplyValueType::Error;
		}
		bool FoundCommand = false;
		ReplyBuffer[ReplyBufferIndex]='\0';
		for (uint8_t Index = 0; Index < static_cast<uint8_t>(CommandsType::Count); Index++)
		{
			if (strcmp(ReplyBuffer,CommandLibrary[Index].String)==0)
			{
				FoundCommand = true;
				CurrentReply.Command = CommandLibrary[Index].Command;
				switch (CommandLibrary[Index].ValueType)
				{
					case CommandValueType::Integer:
						CurrentReply.ValueType = ReplyValueType::Integer;
						break;
					case CommandValueType::Bool:
						CurrentReply.ValueType = ReplyValueType::Bool;
						break;
					default:
						CurrentReply.ValueType = ReplyValueType::Error;
						break;
				}
			}
		}
		if (!FoundCommand)
		{
			CurrentReply.Command = CommandsType::Error;
			CurrentReply.Type = TransmissionType::Error;
			CurrentReply.ValueType = ReplyValueType::Error;
			CurrentRecievingPart = RecievingPart::Echo;
			CheckReply();
		}
		ReplyBufferIndex = 0;
	}
}
void FW102::ParseValue(char Character)
{
	//Serial.print(Character);
	if (isdigit(Character))
	{
		ReplyBuffer[ReplyBufferIndex] = Character;
		if (ReplyBufferIndex < (ReplyBufferSize-1) )
		{
			ReplyBufferIndex++;
		}
	}
	else if ( Character == PromptCharacter )
	{
		if (CurrentReply.ValueType == ReplyValueType::Integer)
		{
			CurrentReply.Value.IntegerValue = CharToUint8t(ReplyBuffer[0]);
		}
		else if (CurrentReply.ValueType == ReplyValueType::Bool)
		{
			if (ReplyBuffer[0] == '1')
			{
				CurrentReply.Value.BoolValue = true;
			}
			else
			{
				CurrentReply.Value.BoolValue = false;
			}
		}
		ReplyBufferIndex = 0;
		CurrentRecievingPart = RecievingPart::Echo;
		CheckReply();
	}
	else if ( (Character == CarriageReturnCharacter) || (Character == EndOfLineCharacter) || (Character == SpaceCharacter) )
	{

	}
	else
	{
		CurrentReply.Command = CommandsType::Error;
		CurrentReply.Type = TransmissionType::Error;
		CurrentReply.ValueType = ReplyValueType::Error;
		CurrentRecievingPart = RecievingPart::Echo;
		CheckReply();
	}
}
void FW102::CheckReply()
{
	ExpectReply = false;
	if (CurrentReply.Command == CommandsType::Error)
	{
		if (CommandRetries < MaxCommandRetries)
		{
			CommandRetries++;
			SendCurrentCommand();
		}
		else
		{
			CommandRetries = 0;
			Serial.println("Filter transmission failed.");
		}
	}
	else
	{
		Busy = false;
		CommandRetries = 0;
		if (RecievedCallback)
		{
			RecievedCallback();
		}
	}
}
uint8_t FW102::GetReplyValue()
{
	if (CurrentReply.ValueType == ReplyValueType::Integer)
	{
		return CurrentReply.Value.IntegerValue;
	}
	else if (CurrentReply.ValueType == ReplyValueType::Bool)
	{
		return (uint8_t)CurrentReply.Value.BoolValue;
	}
	else
	{
		return 254;
	}
}