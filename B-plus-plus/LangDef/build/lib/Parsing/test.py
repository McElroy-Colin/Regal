sys.path.append('..')
from Types.BppTypes import *

def find_encasers(tok_lst, tokens):
    group_count = 0
    groups = []
    encasers = -1
    tok_amt = len(tok_lst)
    for iter1 in range(tok_amt):
        if (check(tok_lst, LeftParen, iter1)):
            group_count += 1
            groups.append(group_count)
        elif (check(tok_lst, RightParen, iter1)):
            group_count -= 1
            groups.append(group_count)
        if ((encasers > -1) and (group_count == 0) and (check_many(tok_lst, tokens, iter1))):
            return (match_bypass_any(tok_lst, tokens)[1])()
    check_back = False
    group_amt = len(groups)
    for iter1 in range(2):
        prev_num = -1
        if (check_back):
            og_groups = groups[:]
            groups.reverse()
        for iter2 in range(group_amt):
            num = groups[iter2]
            if (prev_num > num):
                if (check_back):
                    end_ind = group_amt - iter2
                    break
                else:
                    start_ind = iter2
                    break
            prev_num = num
        check_back = True
    return min(og_groups[start_ind:end_ind])