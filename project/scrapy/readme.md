# 1.virtualenv

环境设置:

1. 建立环境:`virtualenv -p 'python/path' scrapyenv`
2. `cd scrapyenv`,进入环境:`source bin/activate`
3. `cd scrapyenv`,退出环境:`source bin/deactivate`

# 2.ipython

进入ipython环境`ipython`

# 3.scrapy

1. `scrapy startproject myspider`建立工程
2. `scrapy genspider id_key "http://www.example.com"`建立spider
3. `scrapy crawl id_key`开始crawl

# 4.scray shell
查看所有:` scrapy view http://www.example.com/some/page.html`

1. `scrapy shell [url]`
2. `response.css('title').getall()`
3. `response.xpath('//title')`

# 5.scrapy结构

1. Spider:请求url，并返回response，item相关
2. Item:格式化Spider返回的Item
3. ItemPipline:对返回的Item筛选处理

# 6.选择器

1. css
2. xpath