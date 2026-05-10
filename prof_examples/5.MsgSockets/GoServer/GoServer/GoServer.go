package main

import (
	"fmt"
	"log"
	"net"
)

var maxID int32 = MR_USER
var sessions = make(map[int32]*Session)

func processClient(conn net.Conn) {
	defer conn.Close()
	for {
		m := new(Message)
		code := m.Receive(conn)
		fmt.Println(m.Header.To, ":", m.Header.From, ":", m.Header.Type, ":", code)

		switch code {

		case MT_INIT:
			maxID++
			session := Session{maxID, m.Data, make(chan *Message, 10)}
			sessions[session.id] = &session
			MessageSend(conn, session.id, MR_BROKER, MT_INIT, "")

		case MT_EXIT:
			delete(sessions, m.Header.From)
			MessageSend(conn, m.Header.From, MR_BROKER, MT_CONFIRM, "")

		case MT_GETDATA:
			if session, ok := sessions[m.Header.From]; ok {
				session.Send(conn)
			}

		case MT_ERROR:
			return

		default:
			if _, ok := sessions[m.Header.From]; ok {
				if session, ok := sessions[m.Header.To]; ok {
					session.Add(m)
				} else if m.Header.To == MR_ALL {
					for id, session := range sessions {
						if id != m.Header.From {
							session.Add(m)
						}
					}
				}
				MessageSend(conn, m.Header.From, MR_BROKER, MT_CONFIRM, "")
			}
		}
	}
}

func main() {
	l, err := net.Listen("tcp", "localhost:12345")
	if err != nil {
		log.Panicln("Error:", err.Error())
	}
	defer l.Close()
	for {
		conn, err := l.Accept()
		if err != nil {
			log.Panicln("Error:", err.Error())
		}
		go processClient(conn)
	}
}
