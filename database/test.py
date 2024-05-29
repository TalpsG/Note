import re

def split_markdown_by_headers(filename):
    with open(filename, 'r', encoding='utf-8') as file:
        content = file.read()

    # 用正则表达式匹配H1标题
    headers = re.split(r'(?m)^# (.+)$', content)[1:]  # we skip the first split item as it's the content before the first header

    # 检查是否有奇数个元素，每个标题后应该跟着它的内容
    assert len(headers) % 2 == 0, "Headers and content counts do not match."

    for i in range(0, len(headers), 2):
        # 清洁标题用来作为文件名
        title = headers[i].strip().replace('/', '-').replace('\\', '-')
        # 取得下面的内容
        section = headers[i + 1].strip()
        # 创建新的Markdown文件
        with open(f'{title}.md', 'w', encoding='utf-8') as file:
            file.write(f'# {title}\n\n{section}')

# 调用函数传入你的markdown文件名
markdown_file = 'lec.md'  # 替换成你的markdown文件名
split_markdown_by_headers(markdown_file)

