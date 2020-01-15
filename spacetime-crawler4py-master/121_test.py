from bs4 import BeautifulSoup as bs
import requests
from lxml.html.clean import Cleaner
import re
import pickle
from nltk.corpus import stopwords
import nltk
import urllib.parse
dict_path = '/Users/lijie/Desktop/word_count.txt'
longest_path = '/Users/lijie/Desktop/longest.txt'
url_path = '/Users/lijie/Desktop/url_list.txt'
error_url = '/Users/lijie/Desktop/error_url.txt'
test_case = '/Users/lijie/Desktop/test_case.txt'
all_urls ='/Users/lijie/Desktop/all_url.txt'
table = str.maketrans(".,?!'\";:-_(){}[]\|`~#$%^&*<:>/+=","                                ")
def delete_fragment(url:str):
    a = 0
    for char in url:
        if char == '#':
            return url[0:a]
        a+=1
    return url
def tokenize(path:str):
    file = open(path,"r")
    text = file.read()
    ##get all of the text
    text = text.translate(table)
    ##translate the file
    words = text.split()   ##O(n)
    return words
##for this it is O(n)
def computeWordFrequencies(token:list):
    count = {}
    for w in token:   ##O(n)
        if len(w) == 1:
            continue
        word = w.lower()
        ##avoid caps
        if word not in count:
            count[word] = 1
        else:
            count[word] += 1
    return count
##for this it is O(n)
def p(frequence:dict):
    final_list = []
    for k,v in frequence.items():
        final_list.append((k,v))
    ##put all value from dict to list in a set form
    final_list.sort(key = lambda a : a[1],reverse = True)  # the sort it is O(n log n)
    text = ""
    for element in final_list:  ##O(n)
        t = str(element[0]) + " : " + str(element[1]) + "\n"
        text += t
    print(text)
def avoid_repeat(url):
    '''detect if one word appear continously in one url too much time(>= 3 times)
        to filter them
    '''
    a = url.split('/')
    final_dict = {}
    count = 0;
    temp = 0;
    for element in a:
        check_list = a[count:]
        check_count = 0;
        l = len(check_list)
        for check in check_list:
            if (check_count < l-2) and (check == check_list[check_count + 1]):
                temp +=1
            else:
                break
            if temp >= 3:
                return False
        count += 1
        temp = 0
    return True
def combine_dict(a,b):
    for key,value in a.items():
        if key not in b:
            b[key] = value
        else:
            b[key] += value
    return b

def computeWordFrequencies(token:list):
    count = {}
    for w in token:   ##O(n)
        if len(w) == 1:
            continue
        word = w.lower()
        ##avoid caps
        if word not in count:
            count[word] = 1
        else:
            count[word] += 1
    return count
def combine_dict(a,b):
    '''combine two dict {str:int}, addup int
    '''
    for key,value in a.items():
        if key not in b:
            b[key] = value
        else:
            b[key] += value
    return b
def process_html(html):
    '''process html to tokenize to get a list of words
    '''
    table = str.maketrans(".,?!'\";:-_(){}[]\|`~#$%^&*<:>/","                              ")
    reg = re.compile('<[^>]*>')
    html = reg.sub('',html.decode().replace('\n','').replace(' ',''))
    text = html.translate(table)
    words = text.split()
    return words
def count_word(path,words):
    '''tokenize words in html and get a dict of word count
        take local file which contains other dict and combine them and save again
    '''
    temp_dict = computeWordFrequencies(words)
    try:
        file = open(path,'rb')
        d = pickle.load(file)
        file.close()
        final_dict = combine_dict(temp_dict,d)
        file = open(path,'wb')
        pickle.dump(final_dict,file)
        file.close()
    except:
        file.close()
        file = open(path,'wb')
        pickle.dump(temp_dict,file)
        file.close()
def update_longest(words,url):
    length = len(words)
    file = open(longest_path,'r')
    words = file.read().split()
    file.close()
    if length > int(words[0]):
        file = open(longest_path,'w')
        file.write(str(length) + ' ')
        file.write(url)
        file.close()
def update_url(url):
    file = open(url_path,'a')
    urls = []
    for u in url:
        urls.append(delete_fragment(u))
    urls = set(urls)
    for u in urls:
        file.write(u + '\n')
    file.close()

url = 'https://www.ics.uci.edu/about/bren/index.php'
html = requests.get(url).content
words = process_html(html)
count_word(dict_path,words)   ##update the words frequency
update_longest(words,url)   ##updata the longest page
update_url(url)


##get top_50 without stopwords

stopwords = list({'ourselves', 'hers', 'between', 'yourself', 'but', 'again', 'there', 'about', 'once', 'during', 'out', 'very',
                  'having', 'with', 'they', 'own', 'an', 'be', 'some', 'for', 'do', 'its', 'yours', 'such', 'into', 'of', 'most',
                  'itself', 'other', 'off', 'is', 's', 'am', 'or', 'who', 'as', 'from', 'him', 'each', 'the', 'themselves', 'until',
                  'below', 'are', 'we', 'these', 'your', 'his', 'through', 'don', 'nor', 'me', 'were', 'her', 'more', 'himself',
                  'this', 'down', 'should', 'our', 'their', 'while', 'above', 'both', 'up', 'to', 'ours', 'had', 'she', 'all', 'no',
                  'when', 'at', 'any', 'before', 'them', 'same', 'and', 'been', 'have', 'n', 'will', 'on', 'does', 'yourselves', 'then',
                  'that', 'because', 'what', 'over', 'why', 'so', 'can', 'did', 'not', 'now', 'under', 'he', 'you', 'herself', 'has', 'just',
                  'where', 'too', 'only', 'myself', 'which', 'those', 'i', 'after', 'few', 'whom', 't', 'being', 'if', 'theirs', 'my', 'against',
                  'a', 'by', 'doing', 'it', 'how', 'further', 'was', 'here', 'than'})
def get_top_50():
    file = open(dict_path,'rb')
    aa = pickle.load(file)
    file.close()
    b = []
    for key,value in aa.items():
        if key not in stopwords:
            b.append((key,value))
    b.sort(key = lambda x:x[1],reverse = True)
    top_50 = b[:50]
    return top_50
def get_url_number():
    file = open(url_path,'r')
    urls = file.read()
    li = urls.split('\n')
    l = []
    for element in li:
        if len(element) > 5:
            l.append(element)
    file.close()
    return len(l)

def clean_up():
    a = open(dict_path,'w')
    a.write('')
    a.close()

def subdomain():
    file = open(all_urls,'r')
    urls = file.read()
    l = urls.split('\n')
    file.close()
    sub = {}
    for url in l:
        n= urllib.parse.urlparse(url).netloc
        if n not in sub and n != 'www.ics.uci.edu' and n != '' and 'ics.uci.edu' in n:
            sub[n] = 1
        elif n != 'www.ics.uci.edu'and n != '' and 'ics.uci.edu' in n:
            sub[n] += 1
    return sub

def compare_url(checks):
    '''put in one list which contains 1 or 2 urls, compare urls to return string which the parts are same
        '''
    if len(checks) == 1:
        return checks[0]
    first = checks[0].split('/')
    second = checks[1].split('/')
    final_str = ''
    count = 0
    for a in first:
        if count < len(second):
            if a == second[count]:
                final_str += a
                final_str += '/'
            else:
                temp_1 = a
                temp_2 = second[count]
                break
        count +=1
    index = 0
    try:
        for element in temp_1:
            if element == temp_2[index]:
                final_str+=element
            else:
                break
            index +=1
        return final_str
    except:
        return final_str
#    first = checks[0]
#    second = checks[1]
#    final_str = ''
#    count = 0
#    for a in first:
#        if count < len(second):
#            if a == second[count]:
#                final_str += a
#        else:
#            return final_str
    return final_str
def process_error(checks:list):
    start = checks[0].split()[0]
    for element in checks:
        e = element.split()
        if int(e[1]) in (200,300,301,302,303,304,305,306,307) and int(e[2]) > 80:
            return None
        else:
            start = compare_url([start, e[0]])
    if start == 'https://www.ics.uci.edu':
        return None
#    a = start.split('/')
#    other = False
#    for element in a:
#        if element not in ('http','https','www.ics.uci.edu',''):
#            other = True
#    if other == False:
#        return None
    return start

def determine_error():
    '''check local file which contains all urls and find
        if we get continous error from one pattern of urls
        get the similar pattern and store it to local file, so we can filte them
        '''
    file = open('/Users/lijie/Desktop/url_list.txt')
    tt = file.readlines()
    text = []
    for t in tt:
        if(len(t) > 5):
            try:
                int(t[-2])
                text.append(t)
            except:
                continue
    file.close()
    check = text[len(text) - 4:]
    checks = []
    for element in check:
        a = urllib.parse.urlparse(element).netloc
        n= urllib.parse.urlparse(element).path
        checks.append(n)
    error_url = process_error(checks)
    if error_url == None:
        return
    b = urllib.parse.urlparse(error_url)
    if b.netloc == 'www.ics.uci.edu' or b.path == '/' or b.path == '/\n':
        return
    else:
        file = open('/Users/lijie/Desktop/error_url.txt','a')
        file.write(a + error_url + '\n')
        file.close()

    
test = ['https://www.ics.uci.edu/grad/index.php 404',
        'https://www.iscs.uci.edu/grad/index.php 404',
        'https://www.ics.uci.edu/grad/index.php 404',
        'https://www.ics.uci.edu/grad/index.php 404']

##——————————————————————————————————————————————————————————————————————————————

if __name__ == '__main__':
    top = get_top_50()
    url_number = get_url_number()
    sub = subdomain()
    sorted_sub = []
    for key,value in sub.items():
        if key[0] == 'w' or key[1] == 'w':
            key = key[4:]
        sorted_sub.append((key,value))
    sorted_sub.sort(key = lambda x:x[0])
    file = open(dict_path,'rb')
    aa = pickle.load(file)
    file.close()
    file = open('/Users/lijie/Desktop/sample/s_word_count.txt','wb')
    pickle.dump(aa,file)
    file.close()


