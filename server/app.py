from flask import Flask, render_template,request
from flask_sqlalchemy import SQLAlchemy
import time 
app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = 'mysql+pymysql://root:xxxxx@localhost:3306/pp'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db = SQLAlchemy(app)
class FileTable(db.Model):
    __tablename__ = "filelist"
    id = db.Column(db.Integer, primary_key=True)
    filename = db.Column(db.String(120))
    sign = db.Column(db.String(120), unique=True)
    user_ip = db.Column(db.String(120), unique=True)
    createTime = db.Column(db.String(120), default=time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
    def __repr__(self):
        return '<File %r>' % self.filename

@app.route('/ask', methods=['POST'])
def index():
    sign = request.json.get('sign')
    res = db.session.query(FileTable).filter(FileTable.sign == sign ).all()
    print(res)
    if len(res):
        return {"code": 200,"msg":"This file already exists"},302
    # 返回成功响应
    return {'status': 'success'},200

@app.route('/getData', methods=['GET'])
def hello():
    page = int(request.args.get("page"))
    res = db.session.query(FileTable).all()
    result = []
    for i in res[(page-1)*10:(page-1)*10+10]:
        result.append({"id":i.id,"filename":i.filename.split("\\")[-1],"date":i.createTime})
    return {"code": 200,"msg":"success","data":result,"size":len(res)},200
@app.route('/download', methods=['GET'])
def getLoadUrl():
    res = db.session.query(FileTable).filter(FileTable.id==request.args.get("id")).all()
    if not len(res):
        return {"code":200,"msg":"not found!"} , 404
    return {"code":200,"data":{"path":res[0].sign,"filename":res[0].filename.split("\\")[-1]}}
@app.route('/upload', methods=['POST'])
def about():
    user_ip = request.remote_addr
    filename = request.form.get('fileName')
    print(user_ip)
    print(request.files)
    file = request.files['file']
    sign = request.form.get('sign')

    if not filename or not file or not sign:
        return {"code": 400,"msg":"sign is null"},401
    file.save('./upload/'+sign)
    user = FileTable(sign=sign,filename = filename,user_ip = user_ip,)
    db.session.add(user)
    db.session.commit()
    return {"code": 200,"msg":"success"},200

if __name__ == '__main__':
    app.run(debug=True)