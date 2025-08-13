package action

import (
	"fmt"
	"html/template"
	"net/http"
	"signaling/src/framework"
)

type xrtcClientPullAction struct {
}

func NewXrtcClientPullAction() *xrtcClientPullAction {
	return &xrtcClientPullAction{}
}

func (*xrtcClientPullAction) Execute(w http.ResponseWriter, cr *framework.ComRequest) {
	r := cr.R
	//加载模板文件
	staticDir := framework.GetStaticDir()
	t, err := template.ParseFiles(staticDir + "/template/pull.tpl")
	if err != nil {
		fmt.Println(err)
		w.WriteHeader(http.StatusNotFound)
		w.Write([]byte("404 - Not Found"))
		return
	}

	request := make(map[string]string)

	for k, v := range r.Form {
		request[k] = v[0]

	}

	err = t.Execute(w, request)
	if err != nil {
		fmt.Println(err)
		w.WriteHeader(http.StatusNotFound)
		w.Write([]byte("404 - Not Found"))
		return
	}
}
