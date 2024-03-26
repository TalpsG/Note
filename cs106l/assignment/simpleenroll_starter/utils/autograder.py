'''
Hey friend! Glad you're curious about this, but please don't touch this file :>)
'''

from csv_utils import *
import os

path = "/home/talps/gitrepo/Note/cs106l/assignment/simpleenroll_starter"
ans_offer = path+"/utils/courses_offered.bin"
ans_no_offer= path+"/utils/courses_not_offered.bin"
ans_offer_csv = path+"/utils/courses_offered.csv"
ans_no_offer_csv = path+"/utils/courses_not_offered.csv"

talps_ans_offer = path+"/student_output/courses_offered.csv"
talps_ans_no_offer = path+"/student_output/courses_not_offered.csv"



def main():
  # fstream writing tests!
  binary_to_csv(ans_offer, ans_offer_csv)
  binary_to_csv(ans_no_offer,ans_no_offer_csv)
  assert files_are_equal(ans_offer_csv,talps_ans_offer), "write_courses_offered test failed 😞"
  assert files_are_equal(ans_no_offer_csv,talps_ans_no_offer), "write_courses_not_offered test failed 😞"
  os.remove(ans_offer_csv)
  os.remove(ans_no_offer_csv)
  print("Congratulations, your code passes all the autograder tests! ✅")

main()
