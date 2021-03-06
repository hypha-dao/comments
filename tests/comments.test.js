const { loadConfig, Blockchain } = require('@klevoya/hydra');

const config = loadConfig('hydra.yml');

const accountPermission = (account) => [{
  actor: account.accountName,
  permission: 'active'
}];

describe('comments', () => {
  const blockchain = new Blockchain(config);
  const comments = blockchain.createAccount('comments');
  const dao = blockchain.createAccount('dao');

  beforeAll(async () => {
    comments.setContract(blockchain.contractTemplates['comments']);
    comments.updateAuth('active', 'owner', {
      accounts: [
        {
          permission: {
            actor: comments.accountName,
            permission: 'eosio.code'
          },
          weight: 1
        }
      ]
    });
  });

  beforeEach(async () => {
    comments.resetTables();
  });

  it('add section creates the section', async () => {
    await comments.contract.addsection({
      scope: dao.accountName,
      tenant: 'foo',
      section: 'my.section',
      author: 'user1',
    }, [
      {
        actor: dao.accountName,
        permission: 'active'
      }
    ]);

    expect(comments.getTableRowsScoped(`sections`)[dao.accountName]).toMatchObject([
      {
        section: 'my.section',
        tenant: 'foo',
        author: 'user1',
        comments: '0'
      }
    ]);
  });

  it('Can not add the section twice', async () => {
    await comments.contract.addsection({
      scope: dao.accountName,
      section: 'my.section',
      tenant: 'foo',
      author: 'user1',
    }, [
      {
        actor: dao.accountName,
        permission: 'active'
      }
    ]);

    await expect(comments.contract.addsection({
      scope: dao.accountName,
      section: 'my.section',
      tenant: 'foo',
      author: 'user1',
    }, [
      {
        actor: dao.accountName,
        permission: 'active'
      }
    ])).rejects.toThrowError(/Section already exists/i);
  });

  it('Can remove a section', async () => {
    await comments.contract.addsection({
      scope: dao.accountName,
      tenant: 'foo',
      section: 'my.section',
      author: 'user1',
    }, [
      {
        actor: dao.accountName,
        permission: 'active'
      }
    ]);

    await comments.contract.delsection({
      scope: dao.accountName,
      tenant: 'foo',
      section: 'my.section'
    }, [
      {
        actor: dao.accountName,
        permission: 'active'
      }
    ]);
  });

  it('Can add comments', async () => {

    const user1 = blockchain.createAccount('user1');
    const user2 = blockchain.createAccount('user2');
    const user3 = blockchain.createAccount('user3');

    await comments.contract.addsection({
      scope: dao.accountName,
      tenant: 'foo',
      section: 'my.section',
      author: 'user1',
    }, accountPermission(dao));

    await comments.contract.addcomment({
      scope: dao.accountName,
      tenant: 'foo',
      section: 'my.section',
      author: 'user2',
      content: 'Im happy',
      parent_id: null
    }, accountPermission(user2));

    await comments.contract.addcomment({
      scope: dao.accountName,
      tenant: 'foo',
      section: 'my.section',
      author: 'user3',
      content: 'Im sad',
      parent_id: null
    }, accountPermission(user3));

    await comments.contract.addcomment({
      scope: dao.accountName,
      tenant: 'foo',
      section: 'my.section',
      author: 'user3',
      content: 'good for you!',
      parent_id: '1'
    }, accountPermission(user3));

    expect(comments.getTableRowsScoped('comments')[dao.accountName]).toMatchObject([
      {
        id: '1',
        tenant: 'foo',
        parent_id: '0',
        section: 'my.section',
        author: 'user2',
        content: 'Im happy',
        status: 'active'
      },
      {
        id: '2',
        tenant: 'foo',
        parent_id: '0',
        section: 'my.section',
        author: 'user3',
        content: 'Im sad',
        status: 'active'
      },
      {
        id: '3',
        tenant: 'foo',
        parent_id: '1',
        section: 'my.section',
        author: 'user3',
        content: 'good for you!',
        status: 'active'
      }
    ]);


    await comments.contract.editcomment({
      scope: dao.accountName,
      tenant: 'foo',
      author: 'user3',
      comment_id: '2',
      content: '~Im sad~'
    }, accountPermission(user3));

    expect(comments.getTableRowsScoped('comments')[dao.accountName]).toMatchObject([
      {
        id: '1',
        tenant: 'foo',
        parent_id: '0',
        section: 'my.section',
        author: 'user2',
        content: 'Im happy',
        status: 'active'
      },
      {
        id: '2',
        tenant: 'foo',
        parent_id: '0',
        section: 'my.section',
        author: 'user3',
        content: '~Im sad~',
        status: 'active'
      },
      {
        id: '3',
        tenant: 'foo',
        parent_id: '1',
        section: 'my.section',
        author: 'user3',
        content: 'good for you!',
        status: 'active'
      }
    ]);

    await comments.contract.delcomment({
      scope: dao.accountName,
      tenant: 'foo',
      author: 'user3',
      comment_id: '2'
    }, accountPermission(user3));

    expect(comments.getTableRowsScoped('comments')[dao.accountName]).toMatchObject([
      {
        id: '1',
        tenant: 'foo',
        parent_id: '0',
        section: 'my.section',
        author: 'user2',
        content: 'Im happy',
        status: 'active'
      },
      {
        id: '2',
        tenant: 'foo',
        parent_id: '0',
        section: 'my.section',
        author: 'user3',
        content: '',
        status: 'deleted'
      },
      {
        id: '3',
        tenant: 'foo',
        parent_id: '1',
        section: 'my.section',
        author: 'user3',
        content: 'good for you!',
        status: 'active'
      }
    ]);
  });
});
