#define _CRT_SECURE_NO_WARNINGS
#include "VirtualMachine.h"

#define LOPART(_qw)    ((signed long)(_qw))
#define HIPART(_qw)    ((signed long)(((_qw) >> 32) & 0xffffffff))

void CVirutalMachineInterpreter::interpret(const CVirtualMachineInstruction *code)
{
    size_t pc = 0;
    size_t funcBase = 0;
    CVirtualMachineInstruction instruction;
    do
    {
        instruction = code[pc];	/* ����ǰָ�� */
        pc++;
        switch (instruction.inst)
        {
        case VI_NOP:
            // empty instruction, nothing needed to do.
            break;

        case VI_LIT_INTEGER:
            rs.push(instruction.param_b);
            break;

        case VI_LOAD_INTEGER:
            if (instruction.param_a == 0)
                rs.push(rs[instruction.param_b]);
            else if (instruction.param_a == 1)
                rs.push(rs[funcBase + instruction.param_b]);
            else
                exit(EXIT_FAILURE);
            break;

        case VI_LOAD_INTEGER_REF:
            if (instruction.param_a == 1)
            {
                __int64 ref_data = rs[funcBase + instruction.param_b];
                __int32 refBase = LOPART(ref_data);
                __int32 refOffset = HIPART(ref_data);
                rs.push(rs[refBase + refOffset]);
            }
            else
                exit(EXIT_FAILURE);
            break;

        case VI_MAKE_REFERENCE:
        {
            unsigned __int64 refWord = (unsigned __int64)((unsigned long)instruction.param_a);
            refWord |= (unsigned __int64)((unsigned long)instruction.param_b) << 32;
            rs.push(refWord);
            break;
        }

        case VI_STORE_INTEGER:
            if (instruction.param_a == 0)
                rs.store(instruction.param_b);
            else if (instruction.param_a == 1)
                rs.store(funcBase + instruction.param_b);
            else
                exit(EXIT_FAILURE);
            rs.pop();
            break;

        case VI_STORE_INTEGER_REF:
            if (instruction.param_a == 1)
            {
                __int64 ref_data = rs[funcBase + instruction.param_b];
                __int32 refBase = LOPART(ref_data);
                __int32 refOffset = HIPART(ref_data);
                rs.store(refBase + refOffset);
            }
            else
                exit(EXIT_FAILURE);
            rs.pop();
            break;

        case VI_BRANCH:
            rs.push(0);
        case VI_BRANCH_COND:
            if (!rs.top())
                pc = (size_t)instruction.param_b;
            rs.pop();
            break;

        case VI_INITIALIZE_VAR:
            // int 0, count
            rs.init_vars(instruction.param_b);
            break;

        case VI_BRANCH_LINK: //
        {
            //bl 0, ��ַ
            // ��һ��Ϊ�����ֶ�
            size_t newBase = rs.get_top(); // the pos for param_count
            rs.push(pc); // RA
            rs.push(funcBase); // DL
            pc = instruction.param_b;
            funcBase = newBase;
            break;
        }

        case VI_OPERATION:
            switch ((size_t)instruction.param_a)
            {
            case 0: // opr 0, 0 ��������
            {
                size_t newTop = funcBase - rs[funcBase];//params
                pc = rs[funcBase + 1];//RA
                funcBase = rs[funcBase + 2];//last base
                rs.return_pop(newTop);
                break;
            }
            case 1:
                rs[rs.get_top()] = -rs.top();
                break;

            case 2: /* ��ջ�������ջ���������ջԪ�أ����ֵ��ջ */
                rs[rs.get_top() - 1] = rs[rs.get_top() - 1] + rs.top();
                rs.pop();
                break;

            case 3: /* ��ջ�����ȥջ���� */
                rs[rs.get_top() - 1] = rs[rs.get_top() - 1] - rs.top();
                rs.pop();
                break;

            case 4:/* ��ջ�������ջ���� */
                rs[rs.get_top() - 1] = rs[rs.get_top() - 1] * rs.top();
                rs.pop();
                break;

            case 5:/* ��ջ�������ջ���� */
                rs[rs.get_top() - 1] = rs[rs.get_top() - 1] / rs.top();
                rs.pop();
                break;

            case 6:/* ��ջ����ȡģջ���� */
                rs[rs.get_top() - 1] = rs[rs.get_top() - 1] % rs.top();
                rs.pop();
                break;

            case 7:/* ��ջ������ջ�����Ƿ���� */
                rs[rs.get_top() - 1] = rs[rs.get_top() - 1] == rs.top() ? (__int64)-1 : 0;
                rs.pop();
                break;

            case 8:/* ��ջ������ջ�����Ƿ���� */
                rs[rs.get_top() - 1] = rs[rs.get_top() - 1] != rs.top() ? (__int64)-1 : 0;
                rs.pop();
                break;

            case 9:/* ��ջ������ջ�����Ƿ�С�� */
                rs[rs.get_top() - 1] = rs[rs.get_top() - 1] < rs.top() ? (__int64)-1 : 0;
                rs.pop();
                break;

            case 10:/* ��ջ������ջ�����Ƿ�С�ڵ��� */
                rs[rs.get_top() - 1] = rs[rs.get_top() - 1] <= rs.top() ? (__int64)-1 : 0;
                rs.pop();
                break;

            case 11:/* ��ջ������ջ�����Ƿ���� */
                rs[rs.get_top() - 1] = rs[rs.get_top() - 1] > rs.top() ? (__int64)-1 : 0;
                rs.pop();
                break;

            case 12:/* ��ջ������ջ�����Ƿ���ڵ��� */
                rs[rs.get_top() - 1] = rs[rs.get_top() - 1] >= rs.top() ? (__int64)-1 : 0;
                rs.pop();
                break;

            case 13: //odd function
                rs[rs.get_top()] = rs.top() % 2 == 0 ? 0 : (__int64)-1;
                break;

            case 14:/* ջ��ֵ��� */
                printf("%I64d\n", rs.top());
                rs.pop();
                break;

            case 15:/* ����һ����������ջ�� */
            {
                __int64 val;
                printf("?");
                scanf("%I64d", &val);
                rs.push(val);
                break;
            }

            case 16: // not the top value
                rs[rs.get_top()] = rs.top() != 0 ? 0 : (__int64)-1;
                break;
            default:
                // invalid instruction
                exit(EXIT_FAILURE);
            }
            break;
        case VI_HALT:
            goto HALT_EXIT;
        }
    } while (pc != 0);
HALT_EXIT:
    return;
}

// CRuntimeStack Members

CRuntimeStack::CRuntimeStack()
{
    m_stackData.clear();
    // 0 params, RA for the program, reserved.
    m_stackData.resize(3, 0);
}

__int64 & CRuntimeStack::operator[](size_t pos)
{
    return m_stackData[pos];
}

void CRuntimeStack::push(__int64 value)
{
    m_stackData.push_back(value);
}

void CRuntimeStack::store(size_t pos)
{
    m_stackData[pos] = top();
}

void CRuntimeStack::init_vars(size_t count)
{
    m_stackData.resize(m_stackData.size() + count);
}

size_t CRuntimeStack::get_top()
{
    return m_stackData.size() - 1;
}

__int64 CRuntimeStack::top()
{
    return m_stackData[m_stackData.size() - 1];
}

void CRuntimeStack::pop()
{
    m_stackData.pop_back();
}

void CRuntimeStack::return_pop(size_t top_pos)
{
    m_stackData.resize(top_pos);
}
