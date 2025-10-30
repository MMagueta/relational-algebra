(setq *mailbox* nil)

(defun relational-engine-handler (process response)
  ""
  ;; (setq *mailbox* (json-read-from-string response))
  (setq *mailbox* response)
  (let ((response (process-get process :response)))
    (message "Received: %s" response)    
    (delete-process process)))

(setq relational-engine-address "localhost")
(setq relational-engine-port 8080)

(defun relational-client (content)
  ""
  (let ((connection (open-network-stream "relational-engine" "*relational-engine-socket*" relational-engine-address relational-engine-port)))
    (process-put connection :response nil)
    (set-process-filter connection 'relational-engine-handler)
    (process-send-string connection content)))

(setq msg "<?xml version=\"1.0\"?>
<request>
  <command>CREATE_RELATION</command>
  <name>User</name>
</request>")

(setq msg "<?xml version=\"1.0\"?>
<request>
  <command>ADD_TUPLE</command>
  <relation>Employees</relation>
  <attributes>
    <attribute>
      <name>id</name>
      <type>int</type>
      <value>1</value>
    </attribute>
    <attribute>
      <name>name</name>
      <type>string</type>
      <value>Alice</value>
    </attribute>
  </attributes>
</request>")

(setq msg "<?xml version=\"1.0\"?>
<request>
  <command>ADD_TUPLE</command>
  <relation>Employees</relation>
  <attributes>
    <attribute>
      <name>id</name>
      <type>int</type>
      <value>2</value>
    </attribute>
    <attribute>
      <name>name</name>
      <type>string</type>
      <value>Bob</value>
    </attribute>
  </attributes>
</request>")

(setq msg "<?xml version=\"1.0\"?>
<request>
  <command>QUERY_RELATION</command>
  <relation>Employees</relation>
</request>")

(setq msg "<?xml version=\"1.0\"?>
<request>
  <command>LIST_RELATIONS</command>
</request>")

(relational-client msg)

(with-current-buffer "xml"
  (xml-mode)
  (erase-buffer)
  (goto-char (point-min))
  (insert *mailbox*))
