#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

rbtree *new_rbtree(void) { //rb트리 생성
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *t = (node_t *)malloc(sizeof(node_t));
  p -> nil = t;
  p -> nil -> color = RBTREE_BLACK;
  p -> root = p -> nil;
  return p;
}

void delete_nodes(node_t *node, node_t *nil) {
  if (node == nil) {
    return;
  } 
  delete_nodes(node->left, nil);
  delete_nodes(node->right, nil);
  free(node);
}

void delete_rbtree(rbtree *t) { //rb트리 완전삭제
  delete_nodes(t -> root, t -> nil);
  free(t->nil);
  free(t);
}

void left_rotate(rbtree *t, node_t *x) { //x노드 기준 좌측회전
  node_t *y = x->right;
  x->right = y->left;
  if (y->left != t->nil) {
    y->left->parent = x;
  }
  y->parent = x->parent;

  if (x->parent == t->nil) {
    t->root = y;
  }
  else if (x == x->parent->left) {
    x->parent->left = y;
  }
  else x->parent->right = y;
  y->left = x;
  x->parent = y;
}

void right_rotate(rbtree *t, node_t *x) { //x노드 기준 우측회전
  node_t *y = x->left;
  x->left = y->right;
  if (y->right != t->nil) {
    y->right->parent = x; 
  }
  y->parent = x->parent;

  if (x->parent == t->nil) {
    t->root = y;
  }
  else if (x == x->parent->right) {
    x->parent->right = y;
  }
  else x->parent->left = y;
  y->right = x;
  x->parent = y;
}

void rbtree_insert_fixup(rbtree *t, node_t *z) {//insert 이후 수정작업
  node_t *y;
  while (z->parent->color == RBTREE_RED) {
    if (z->parent == z->parent->parent->left) { //부모가 할배의 좌자식일때
      y = z->parent->parent->right; // y = 삼촌
      if (y->color == RBTREE_RED) { // 삼촌 레드일때(Case1)
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else if (z == z->parent->right) { // 삼촌 블랙이고 우자식일때(Case2)
        z = z->parent;
        left_rotate(t, z);
      }
      else { // 삼촌 블랙이고 좌자식일때(Case3)
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }

    else { //부모가 할배의 우자식일때
      y = z->parent->parent->left;
      if (y->color == RBTREE_RED) { // Case1-right
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else if (z == z->parent->left) { // Case2-right
        z = z->parent;
        right_rotate(t, z);
      }
      else { // Case3-right
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  //일단 넣는 단계. 색깔규칙 일단 무시. 이진검색트리와 동일.
  node_t *y = t->nil;
  node_t *x = t->root;
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  z->color = RBTREE_RED;
  z->key = key;
  z->left = t->nil;
  z->right = t->nil;
  while(x !=NULL && x != t->nil) {
    y = x;
    if (z->key < x->key) x = x->left;
    else x = x->right;
  }
  
  z->parent = y;
  
  if (y == t->nil) {
    t->root = z;
  }
  else if (z->key < y->key) {
    y->left = z;
  }
  else {
    y->right = z;
  }
  rbtree_insert_fixup(t, z);   //수정단계
  return z;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *p = t -> root;
  while (p != t->nil) {
    if (p -> key == key) return p;
    else if (p -> key < key) p = p -> right;
    else p = p -> left;
  }
  return NULL;  //존재하지 않는 키를 받은 경우.
}

node_t *rbtree_min(const rbtree *t) {//아래 rbtree_max와 같이, 이진탐색트리와 동일.
  node_t *p = t -> root;
  while (p -> left != t->nil) {
    p = p -> left;
  }
  return p;
}

node_t *rbtree_max(const rbtree *t) {
 node_t *p = t -> root;
  while (p -> right != t->nil) {
    p = p -> right;
  }
  return p;
}

node_t *find_successor(rbtree *t, node_t *p) {//successor를 찾는 함수.
  while (p -> left != t->nil) {
    p = p -> left;
  }
  return p;
}


void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {//erase과정중 이식과정.
  if (u->parent == t->nil) t->root = v;
  else if (u == u->parent->left) u->parent->left = v;
  else u->parent->right = v;
  v->parent = u->parent;
}

void rb_erase_fixup(rbtree *t, node_t* x) {
  node_t *w;
  while (x!=t->root && x->color == RBTREE_BLACK) {
    if (x == x->parent->left) {
      w = x->parent->right;
      if (w->color == RBTREE_RED) { //Case1
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) { //Case2
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else if (w->right->color == RBTREE_BLACK) { //Case3
        w->left->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        right_rotate(t, w);
        w = x->parent->right;
      }
      else { //Case4
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    else {//우대칭일 경우.
      w = x->parent->left;
      if (w->color == RBTREE_RED) { //Case1
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) { //Case2
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else if (w->left->color == RBTREE_BLACK) { //Case3
        w->right->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        left_rotate(t, w);
        w = x->parent->left;
      }
      else { //Case4
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *z) {//색 상관없이 삭제하는 함수.
  node_t *x;
  node_t *y = z;
  int y_ori_color = y->color;
  if (z->left == t->nil) {
    x = z->right;
    rbtree_transplant(t, z, z->right);
  } 
  else if (z->right == t->nil) {
    x = z->left;
    rbtree_transplant(t, z, z->left);
  }
  else {
    y = find_successor(t, z->right);
    y_ori_color = y->color;
    x = y->right;
    if (y->parent == z){
      x->parent = y;
    }
    else{
      rbtree_transplant(t, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    rbtree_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }
  if (y_ori_color == RBTREE_BLACK) {
    rb_erase_fixup(t, x);
  }
  free(z);
  return 1;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
// rb트리를 작은수부터 순서대로 배열로 만드는 함수
  int i = 0;
  void find_ascending(node_t *p) {
    if (p == t->nil) {
      return;
    }
    find_ascending(p->left); //중위 순회 방식으로 맨 좌측아래부터 배열에 넣는다.
    arr[i] = p->key;
    i++;
    find_ascending(p->right);
  }

  find_ascending(t->root);
  return 0;

}
