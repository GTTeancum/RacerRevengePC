#!/usr/bin/env python3
"""Focused authored spring setup and actual-query response checks."""
import sys
sys.dont_write_bytecode=True
import check_static_contact_response50e as previous
runner=previous.runner
runner.SUITE_NAME="spring-authoring50f"
runner.TESTS={**runner.TESTS,
 "spring_authoring50f":{
  "source":"tests/native/spring_authoring50f.cpp",
  "fixture":"evidence/springAuthoring50F/setter-fixtures.json",
  "arguments":["work/impactAuthoring49Y/Pod01.phy","evidence/springAuthoring50F/setter-fixtures.json"]},
 "authored_static_contact50f":{
  "source":"tests/native/static_contact_connected50e.cpp",
  "fixture":"evidence/staticContactResponse50E/actual-query-subset.json",
  "arguments":["evidence/staticContactResponse50E/actual-query-subset.json","work/impactAuthoring49Y/Pod01.phy"]}}
runner.EXTRA_INPUT_PATHS=(*runner.EXTRA_INPUT_PATHS,"tools/check_spring_authoring50f.py","work/impactAuthoring49Y/Pod01.phy")
if __name__=="__main__":sys.exit(runner.main())
