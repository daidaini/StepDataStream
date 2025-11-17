#include "CachedGatePBStep.h"

#include "STD.h"
#include "StringFunc.h"

#include <algorithm>

GatePBStep::GatePBStep(int blockSize)
    : CachedPBStep(blockSize)
{
}

bool GatePBStep::SetPackageHead(const std::string &src)
{
    size_t firstDelimIndex = src.find_first_of('\n');
    if (firstDelimIndex == src.npos)
    {
        return false;
    }

    Init();
    ParseBaseRecord(src.substr(0, firstDelimIndex));
    return true;
}

void GatePBStep::SetBaseRecord(int funcno, int errcode, const char *errmsg)
{
    SetBaseFieldValueInt(STEP_FUNC, funcno);
    SetBaseFieldValueInt(STEP_CODE, errcode);
    SetBaseFieldValueString(STEP_MSG, errmsg);
    SetBaseFieldValueInt(STEP_RETURNNUM, 1);
    SetBaseFieldValueInt(STEP_TOTALNUM, 1);
}

void GatePBStep::SetBaseRecord(int funcno, int recordCnt)
{
    SetBaseFieldValueInt(STEP_FUNC, funcno);
    SetBaseFieldValueInt(STEP_CODE, 0);
    SetBaseFieldValueInt(STEP_RETURNNUM, recordCnt);
    SetBaseFieldValueInt(STEP_TOTALNUM, recordCnt);
}

int GatePBStep::GetBaseFieldValueInt(int stepid)
{
    return atoi(GetBaseFieldValue(stepid).c_str());
}

int GatePBStep::GetFieldValueInt(int stepid)
{
    return atoi(GetItem(stepid).c_str());
}

void GatePBStep::GetFieldValueString(int stepid, char *out, int outlen)
{
    strncpy(out, GetStepValueByID(stepid).c_str(), outlen-1);
}

double GatePBStep::GetFieldValueDouble(int stepid)
{
    return atof(GetItem(stepid).c_str());
}

char GatePBStep::GetFieldValueChar(int stepid)
{
    auto value = GetItem(stepid);
    return value.empty() ? '\0' : value.front();
}

int GatePBStep::FunctionId()
{
    return GetBaseFieldValueInt(STEP_FUNC);
}

int GatePBStep::Requestno()
{
    return GetBaseFieldValueInt(STEP_REQUESTNO);
}


void GatePBStep::AddFieldValueDouble(int stepid, double value, int dot)
{
    char tmp[128]{};
    STD::DoubleToString(tmp, sizeof(tmp), value, dot);
    AddFieldValue(stepid, tmp);
}

void GatePBStep::AddMoneyValue(int stepid, double value)
{
    this->AddFieldValueDouble(stepid, value, 2);
}

void GatePBStep::AddMoneyValue(int stepid, const std::string &value)
{
    this->AddFieldValueDouble(stepid, atof(value.c_str()), 2);
}

void GatePBStep::AddPriceValue(int stepid, double value)
{
    this->AddFieldValueDouble(stepid, value, 4);
}

