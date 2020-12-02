import requests
from bs4 import BeautifulSoup
from collections import OrderedDict
import os


outputFile='108062593_hw1_output.txt'

def read_input():
    with open('input_hw1.txt','r') as fp:
        input_list=fp.readlines()
    for i in range(len(input_list)):
        input_list[i]=input_list[i].strip()
    return input_list


def save_user_record(record):
    with open(outputFile,'a',encoding='utf8') as fp:
        for i in range(len(record)):
            if i==len(record)-1:
                fp.write(record[i]+'\n')
            else:
                fp.write(record[i]+' -> ')
        fp.write('--------------------------------------------------------------------------\n')


class Crawler():
    def __init__(self):
        pass
    def set_user(self,user):
        self.user=user
        self.url="https://www.blockchain.com/eth/address/{}?view=standard".format(self.user)
    def request(self):
        self.res=requests.get(self.url)
        self.html=BeautifulSoup(self.res.text)
    def parse(self):
        # 找現在頁面所需的資料
        parse=self.html.find_all("span",class_="sc-1ryi78w-0 bFGdFC sc-16b9dsl-1 iIOvXh u3ufsr-0 gXDEBk")
        self.data=OrderedDict()
#         self.data['Hash']=parse[0].string
        self.data['Nonce']=parse[1].string
        self.data['Number of Transactions']=parse[2].string
        self.data['Final Balance']=parse[3].string
        self.data['Total Sent']=parse[4].string
        self.data['Total Received']=parse[5].string
        self.data['Total Fees']=parse[6].string
        
        # 找下個要搜尋的customer
        # 如果找不到則回傳None
        amount=self.html.find_all('span',class_='sc-1ryi78w-0 bFGdFC sc-16b9dsl-1 iIOvXh u3ufsr-0 gXDEBk sc-85fclk-0 gskKpd')
        if amount != []:
            amount=amount[-1]
        else:
            amount=None
        # 找下個要搜尋的customer
        self.next_customer=None
        if amount is not None:
            parent=amount.find_parents()[4]
            self.data['Date']=parent.find('span',class_='sc-1ryi78w-0 bFGdFC sc-16b9dsl-1 iIOvXh u3ufsr-0 gXDEBk').string
            self.data['To']=parent.find_all('a',class_='sc-1r996ns-0 dEMBMB sc-1tbyx6t-1 gzmhhS iklhnl-0 dVkJbV')[2].string
            self.data['Amount']=amount.string
            self.next_customer=self.data['To']
            
    def save(self):
        with open(outputFile,'a',encoding='utf8') as fp:
            for key,value in self.data.items():
                fp.write(key + ": " + value+'\n')
            fp.write('--------------------------------------------------------------------------\n')
            
if __name__ == "__main__":
    
    if os.path.exists(outputFile):
        os.remove(outputFile)

    inputs=read_input()
    for _input in inputs:
        crawler=Crawler()
        # 爬取起始page
        crawler.set_user(_input)
        crawler.request()
        crawler.parse()
        crawler.save()
        user_record=[_input]
        # 爬接下來3個page
        for i in range(3):
            if crawler.next_customer is not None:
                user_record.append(crawler.next_customer)
                crawler.set_user(crawler.next_customer)
                crawler.request()
                crawler.parse()
                crawler.save()
            else:
                break
        save_user_record(user_record)